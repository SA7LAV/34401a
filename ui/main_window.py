from PyQt5.QtWidgets import (
    QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QPushButton, QStatusBar, QMessageBox, QLabel
)
from translations import tr, language_changed
from config import app_config
from instrument import Instrument
from models import MeasMode, MODES
from ui.display_widget import DisplayWidget
from ui.mode_panel import ModePanel
from ui.range_panel import RangePanel
from ui.connection_dialog import ConnectionDialog
from ui.setup_dialog import SetupDialog
from ui.overlay_window import OverlayWindow

BTN = ("QPushButton { background: #1A2040; color: #00C0FF; "
       "border: 1px solid #00C0FF; border-radius: 4px; padding: 6px 14px; }"
       "QPushButton:hover { background: #203060; }"
       "QPushButton:disabled { color: #444; border-color: #444; }")
STOP_BTN = ("QPushButton { background: #400A0A; color: #FF4444; "
            "border: 1px solid #FF4444; border-radius: 4px; padding: 6px 14px; }"
            "QPushButton:hover { background: #601010; }")

AUTO_RANGE_CMD = {
    MeasMode.VDC:    "VOLT:DC:RANG:AUTO ON",
    MeasMode.ADC:    "CURR:DC:RANG:AUTO ON",
    MeasMode.VAC:    "VOLT:AC:RANG:AUTO ON",
    MeasMode.AAC:    "CURR:AC:RANG:AUTO ON",
    MeasMode.OHM2:   "RES:RANG:AUTO ON",
    MeasMode.OHM4:   "FRES:RANG:AUTO ON",
    MeasMode.FREQ:   "FREQ:VOLT:RANG:AUTO ON",
    MeasMode.PERIOD: "PER:VOLT:RANG:AUTO ON",
}

NPLC_CMD = {
    MeasMode.VDC:    "SENS:VOLT:DC:NPLC {n}",
    MeasMode.ADC:    "SENS:CURR:DC:NPLC {n}",
    MeasMode.VAC:    "SENS:VOLT:AC:NPLC {n}",
    MeasMode.AAC:    "SENS:CURR:AC:NPLC {n}",
    MeasMode.OHM2:   "SENS:RES:NPLC {n}",
    MeasMode.OHM4:   "SENS:FRES:NPLC {n}",
}


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setMinimumSize(820, 500)
        self.setStyleSheet("background-color: #0A0A1A;")
        self._instrument = Instrument(self)
        self._current_mode = MeasMode.VDC
        self._overlay = None
        self._connect_port = ""
        self._connect_baud = ""
        self._build_ui()
        self._wire_signals()
        self._apply_overlay(app_config.overlay_enabled)

    def _build_ui(self):
        central = QWidget()
        self.setCentralWidget(central)
        root = QHBoxLayout(central)

        left = QVBoxLayout()
        self._lbl_function = QLabel(tr("label_function"))
        self._lbl_function.setStyleSheet("color: #6080A0; font-size: 10px;")
        left.addWidget(self._lbl_function)
        self._mode_panel = ModePanel()
        left.addWidget(self._mode_panel)
        left.addStretch()
        root.addLayout(left)

        center = QVBoxLayout()
        self._display = DisplayWidget()
        center.addWidget(self._display, stretch=3)
        self._lbl_range = QLabel(tr("label_range"))
        self._lbl_range.setStyleSheet("color: #6080A0; font-size: 10px;")
        center.addWidget(self._lbl_range)
        self._range_panel = RangePanel()
        center.addWidget(self._range_panel)
        root.addLayout(center, stretch=1)

        right = QVBoxLayout()
        self._setup_btn = QPushButton(tr("btn_setup"))
        self._setup_btn.setStyleSheet(BTN)
        self._connect_btn = QPushButton(tr("btn_connect"))
        self._connect_btn.setStyleSheet(BTN)
        self._stop_btn = QPushButton(tr("btn_stop"))
        self._stop_btn.setStyleSheet(STOP_BTN)
        self._stop_btn.setEnabled(False)
        self._local_btn = QPushButton(tr("btn_local"))
        self._local_btn.setStyleSheet(BTN)
        self._local_btn.setEnabled(False)
        right.addWidget(self._setup_btn)
        right.addWidget(self._connect_btn)
        right.addWidget(self._stop_btn)
        right.addWidget(self._local_btn)
        right.addStretch()
        root.addLayout(right)

        status = QStatusBar()
        status.setStyleSheet("color: #6080A0;")
        status.showMessage(tr("status_not_connected"))
        self.setStatusBar(status)
        self._status = status

        self.setWindowTitle(tr("window_title"))

    def _wire_signals(self):
        self._instrument.measurement_received.connect(self._display.update_value)
        self._instrument.error_occurred.connect(self._on_error)
        self._instrument.connected.connect(self._on_connected)
        self._instrument.connection_failed.connect(self._on_connection_failed)
        self._mode_panel.mode_changed.connect(self._on_mode_changed)
        self._range_panel.range_selected.connect(self._on_range_selected)
        self._setup_btn.clicked.connect(self._on_setup_clicked)
        self._connect_btn.clicked.connect(self._on_connect_clicked)
        self._stop_btn.clicked.connect(self._on_stop_clicked)
        self._local_btn.clicked.connect(self._on_local_clicked)
        language_changed.connect(self.retranslate_ui)
        self._mode_panel.select(MeasMode.VDC)

    def retranslate_ui(self) -> None:
        self.setWindowTitle(tr("window_title"))
        self._lbl_function.setText(tr("label_function"))
        self._lbl_range.setText(tr("label_range"))
        self._setup_btn.setText(tr("btn_setup"))
        self._stop_btn.setText(tr("btn_stop"))
        self._local_btn.setText(tr("btn_local"))
        if self._instrument.is_connected:
            self._connect_btn.setText(tr("btn_disconnect"))
        else:
            self._connect_btn.setText(tr("btn_connect"))
            self._status.showMessage(tr("status_not_connected"))
        self._display.retranslate_ui()

    def _on_setup_clicked(self):
        dlg = SetupDialog(self)
        dlg.exec_()
        self._display.apply_display_config()
        self._apply_overlay(app_config.overlay_enabled)
        if self._overlay is not None:
            self._overlay.apply_style()

    def _apply_overlay(self, enabled: bool) -> None:
        if enabled:
            if self._overlay is None:
                self._overlay = OverlayWindow()
                self._instrument.measurement_received.connect(self._overlay.update_value)
            self._overlay.set_unit(MODES[self._current_mode].unit)
            self._overlay.show()
        else:
            if self._overlay is not None:
                self._instrument.measurement_received.disconnect(self._overlay.update_value)
                self._overlay.close()
                self._overlay = None

    def _on_error(self, msg: str):
        self._status.showMessage(tr("status_error", msg=msg))
        self._on_stop_clicked()

    def _on_mode_changed(self, mode):
        self._current_mode = mode
        self._range_panel.set_mode(mode)
        self._display.set_unit(MODES[mode].unit)
        self._display.reset_stats()
        if self._overlay is not None:
            self._overlay.set_unit(MODES[mode].unit)
        if self._instrument.is_connected:
            self._instrument.send_command(MODES[mode].conf_cmd)
            self._send_nplc(mode)

    def _send_nplc(self, mode):
        cmd_tpl = NPLC_CMD.get(mode)
        if cmd_tpl:
            self._instrument.send_command(cmd_tpl.format(n=app_config.nplc))

    def _on_range_selected(self, scpi_cmd):
        if not self._instrument.is_connected:
            return
        if scpi_cmd:
            self._instrument.send_command(scpi_cmd)
        else:
            cmd = AUTO_RANGE_CMD.get(self._current_mode)
            if cmd:
                self._instrument.send_command(cmd)

    def _on_connect_clicked(self):
        if self._instrument.is_connected:
            self._instrument.disconnect()
            self._connect_btn.setText(tr("btn_connect"))
            self._connect_btn.setEnabled(True)
            self._stop_btn.setEnabled(False)
            self._local_btn.setEnabled(False)
            self._status.showMessage(tr("status_disconnected"))
            return

        dlg = ConnectionDialog(self)
        if dlg.exec_() != dlg.Accepted:
            return

        self._connect_port = dlg.port
        self._connect_baud = dlg.baudrate
        self._connect_btn.setEnabled(False)
        self._status.showMessage(tr("status_connecting"))

        az = "ON" if app_config.autozero else "OFF"
        nplc_cmd = NPLC_CMD.get(self._current_mode, "")
        nplc_cmd = nplc_cmd.format(n=app_config.nplc) if nplc_cmd else ""
        setup_cmds = [c for c in [
            "SYST:REM",
            f"SENS:ZERO:AUTO {az}",
            MODES[self._current_mode].conf_cmd,
            nplc_cmd,
        ] if c]

        self._instrument.connect_async(
            dlg.port, dlg.baudrate, dlg.parity, dlg.stopbits,
            bytesize=dlg.bytesize, timeout=dlg.timeout,
            setup_cmds=setup_cmds,
        )

    def _on_connected(self):
        self._connect_btn.setText(tr("btn_disconnect"))
        self._connect_btn.setEnabled(True)
        self._stop_btn.setEnabled(True)
        self._local_btn.setEnabled(True)
        self._status.showMessage(
            tr("status_connected", port=self._connect_port, baudrate=self._connect_baud)
        )

    def _on_connection_failed(self, msg: str):
        self._connect_btn.setEnabled(True)
        self._status.showMessage(tr("status_not_connected"))
        QMessageBox.critical(self, tr("dlg_conn_error_title"), msg)

    def _on_stop_clicked(self):
        self._instrument.stop_sampling()
        self._stop_btn.setEnabled(False)
        self._status.showMessage(tr("status_stopped"))

    def _on_local_clicked(self):
        if self._instrument.is_connected:
            self._instrument.send_command("SYST:LOC")

    def closeEvent(self, event):
        self._apply_overlay(False)
        self._instrument.stop_sampling()
        if self._instrument.is_connected:
            self._instrument.disconnect()
        event.accept()
