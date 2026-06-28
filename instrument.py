import serial
import serial.tools.list_ports
import threading
import time
from typing import Optional
from PyQt5.QtCore import QObject, pyqtSignal


class Instrument(QObject):
    """Kapselt die serielle SCPI-Kommunikation mit dem HP 34401A."""

    measurement_received = pyqtSignal(float)
    error_occurred = pyqtSignal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        self._port: Optional[serial.Serial] = None
        self._thread: Optional[threading.Thread] = None
        self._running = False
        self._lock = threading.Lock()

    @property
    def is_connected(self) -> bool:
        return self._port is not None and self._port.is_open

    def connect(self, port: str, baudrate: int, parity: str,
                stopbits: float, bytesize: int = 8, timeout: float = 9.0) -> None:
        parity_map = {"N": serial.PARITY_NONE, "E": serial.PARITY_EVEN,
                      "O": serial.PARITY_ODD}
        stopbits_map = {1: serial.STOPBITS_ONE, 1.5: serial.STOPBITS_ONE_POINT_FIVE,
                        2: serial.STOPBITS_TWO}
        self._port = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=bytesize,
            parity=parity_map.get(parity, serial.PARITY_NONE),
            stopbits=stopbits_map.get(stopbits, serial.STOPBITS_ONE),
            timeout=timeout,
            write_timeout=4.0,
        )

    def disconnect(self) -> None:
        self.stop_sampling()
        if self._port and self._port.is_open:
            try:
                self._send_raw("SYST:LOC")
            except Exception:
                pass
            self._port.close()
        self._port = None

    def _send_raw(self, cmd: str) -> None:
        with self._lock:
            self._port.write(f"{cmd}\r\n".encode())

    def _readline(self) -> str:
        with self._lock:
            return self._port.readline().decode().strip()

    def query(self, cmd: str) -> str:
        self._send_raw(cmd)
        return self._readline()

    def send_command(self, cmd: str) -> None:
        self._send_raw(cmd)

    def start_sampling(self) -> None:
        if self._running:
            return
        self._running = True
        self._thread = threading.Thread(target=self._sampling_loop, daemon=True)
        self._thread.start()

    def stop_sampling(self) -> None:
        self._running = False
        if self._thread:
            self._thread.join(timeout=2.0)
            self._thread = None

    def _sampling_loop(self) -> None:
        while self._running and self.is_connected:
            try:
                response = self.query("READ?")
                if response and "E+37" not in response:
                    self.measurement_received.emit(float(response))
            except (serial.SerialException, ValueError, OSError) as e:
                self.error_occurred.emit(str(e))
                break
            time.sleep(0.05)

    @staticmethod
    def list_ports() -> list:
        return [p.device for p in serial.tools.list_ports.comports()]
