import queue
import serial
import serial.tools.list_ports
import threading
import time
from typing import Optional
from PyQt5.QtCore import QObject, pyqtSignal
from config import app_config


class Instrument(QObject):
    measurement_received = pyqtSignal(float)
    error_occurred       = pyqtSignal(str)
    connected            = pyqtSignal()
    connection_failed    = pyqtSignal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        self._port:       Optional[serial.Serial]  = None
        self._thread:     Optional[threading.Thread] = None
        self._stop_event  = threading.Event()
        self._cmd_queue:  queue.Queue = queue.Queue()

    @property
    def is_connected(self) -> bool:
        return self._port is not None and self._port.is_open

    # ------------------------------------------------------------------
    # Public API
    # ------------------------------------------------------------------

    def connect_async(self, port: str, baudrate: int, parity: str,
                      stopbits: float, bytesize: int = 8,
                      timeout: float = 9.0,
                      setup_cmds: list = None) -> None:
        """Open serial port and start sampling in a background thread.

        setup_cmds are sent directly after the port opens, before the
        first READ? — no queue delay.  On success, emits connected();
        on failure, emits connection_failed(msg).
        """
        parity_map = {
            "N": serial.PARITY_NONE,
            "E": serial.PARITY_EVEN,
            "O": serial.PARITY_ODD,
        }
        stopbits_map = {
            1:   serial.STOPBITS_ONE,
            1.5: serial.STOPBITS_ONE_POINT_FIVE,
            2:   serial.STOPBITS_TWO,
        }

        def _worker():
            try:
                self._port = serial.Serial(
                    port=port,
                    baudrate=baudrate,
                    bytesize=bytesize,
                    parity=parity_map.get(parity, serial.PARITY_NONE),
                    stopbits=stopbits_map.get(stopbits, serial.STOPBITS_ONE),
                    timeout=timeout,
                    write_timeout=4.0,
                    rtscts=False,
                    dsrdtr=False,
                    xonxoff=False,
                )
                self._port.reset_input_buffer()
                for cmd in (setup_cmds or []):
                    self._port.write(f"{cmd}\r\n".encode())
                # Give device time to process CONF (mode switch + beep) before first READ?
                time.sleep(0.3)
                self._port.reset_input_buffer()
                self.connected.emit()
                self._sampling_loop()
            except Exception as e:
                self._port = None
                self.connection_failed.emit(str(e))

        self._stop_event.clear()
        # drain leftover commands from a previous session
        while not self._cmd_queue.empty():
            try:
                self._cmd_queue.get_nowait()
            except queue.Empty:
                break
        self._thread = threading.Thread(target=_worker, daemon=True)
        self._thread.start()

    def disconnect(self) -> None:
        """Stop sampling, send SYST:LOC, close port."""
        self.stop_sampling()
        if self._port and self._port.is_open:
            try:
                self._port.write("SYST:LOC\r\n".encode())
            except Exception:
                pass
            self._port.close()
        self._port = None

    def send_command(self, cmd: str) -> None:
        """Queue a SCPI command for the sampling thread to send."""
        self._cmd_queue.put(cmd)

    def stop_sampling(self) -> None:
        self._stop_event.set()
        if self._port:
            try:
                # shorten timeout so readline() unblocks quickly
                self._port.timeout = 0.5
            except Exception:
                pass
        if self._thread:
            self._thread.join(timeout=3.0)
            self._thread = None

    # ------------------------------------------------------------------
    # Internal
    # ------------------------------------------------------------------

    def _sampling_loop(self) -> None:
        while not self._stop_event.is_set() and self.is_connected:
            # drain command queue before each measurement
            had_conf = False
            while True:
                try:
                    cmd = self._cmd_queue.get_nowait()
                    self._port.write(f"{cmd}\r\n".encode())
                    if cmd.startswith("CONF:") or cmd.startswith("MEAS:"):
                        had_conf = True
                except queue.Empty:
                    break

            if had_conf:
                # Wait for device to finish mode switch (beep) before issuing READ?
                time.sleep(0.3)
                self._port.reset_input_buffer()

            try:
                self._port.write("READ?\r\n".encode())
                response = self._port.readline().decode().strip()
                if response and "E+37" not in response:
                    self.measurement_received.emit(float(response))
            except (serial.SerialException, ValueError, OSError) as e:
                self.error_occurred.emit(str(e))
                break

            time.sleep(app_config.sampling_ms / 1000.0)

    @staticmethod
    def list_ports() -> list:
        return [p.device for p in serial.tools.list_ports.comports()]
