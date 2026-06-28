from PyQt5.QtWidgets import (
    QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QPushButton, QStatusBar, QMessageBox, QLabel
)
from instrument import Instrument
from models import MeasMode, MODES
from ui.display_widget import DisplayWidget
from ui.mode_panel import ModePanel
from ui.range_panel import RangePanel
from ui.connection_dialog import ConnectionDialog

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


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("HP 34401A – Multimeter Control")
        self.setMinimumSize(820, 500)
        self.setStyleSheet("background-color: #0A0A1A;")
        self._instrument = Instrument(self)
        self._current_mode = MeasMode.VDC
        self._build_ui()
        self._wire_signals()

    def _build_ui(self):
        central = QWidget()
        self.setCentralWidget(central)
        root = QHBoxLayout(central)

        left = QVBoxLayout()
        lbl = QLabel("FUNKTION")
        lbl.setStyleSheet("color: #6080A0; font-size: 10px;")
        left.addWidget(lbl)
        self._mode_panel = ModePanel()
        left.addWidget(self._mode_panel)
        left.addStretch()
        root.addLayout(left)

        center = QVBoxLayout()
        self._display = DisplayWidget()
        center.addWidget(self._display, stretch=3)
        rlbl = QLabel("BEREICH")
        rlbl.setStyleSheet("color: #6080A0; font-size: 10px;")
        center.addWidget(rlbl)
        self._range_panel = RangePanel()
        center.addWidget(self._range_panel)
        root.addLayout(center, stretch=1)

        right = QVBoxLayout()
        self._connect_btn = QPushButton("Verbinden")
        self._connect_btn.setStyleSheet(BTN)
        self._stop_btn = QPushButton("Stop")
        self._stop_btn.setStyleSheet(STOP_BTN)
        self._stop_btn.setEnabled(False)
        self._local_btn = QPushButton("Local")
        self._local_btn.setStyleSheet(BTN)
        self._local_btn.setEnabled(False)
        right.addWidget(self._connect_btn)
        right.addWidget(self._stop_btn)
        right.addWidget(self._local_btn)
        right.addStretch()
        root.addLayout(right)

        status = QStatusBar()
        status.setStyleSheet("color: #6080A0;")
        status.showMessage("Nicht verbunden")
        self.setStatusBar(status)
        self._status = status

    def _wire_signals(self):
        self._instrument.measurement_received.connect(self._display.update_value)
        self._instrument.error_occurred.connect(self._on_error)
        self._mode_panel.mode_changed.connect(self._on_mode_changed)
        self._range_panel.range_selected.connect(self._on_range_selected)
        self._connect_btn.clicked.connect(self._on_connect_clicked)
        self._stop_btn.clicked.connect(self._on_stop_clicked)
        self._local_btn.clicked.connect(self._on_local_clicked)
        self._mode_panel.select(MeasMode.VDC)

    def _on_error(self, msg: str):
        self._status.showMessage(f"Fehler: {msg}")
        self._on_stop_clicked()

    def _on_mode_changed(self, mode):
        self._current_mode = mode
        self._range_panel.set_mode(mode)
        self._display.set_unit(MODES[mode].unit)
        self._display.reset_stats()
        if self._instrument.is_connected:
            self._instrument.send_command(MODES[mode].conf_cmd)

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
            self._instrument.stop_sampling()
            self._instrument.disconnect()
            self._connect_btn.setText("Verbinden")
            self._stop_btn.setEnabled(False)
            self._local_btn.setEnabled(False)
            self._status.showMessage("Getrennt")
            return

        dlg = ConnectionDialog(self)
        if dlg.exec_() != dlg.Accepted:
            return

        try:
            self._instrument.connect(dlg.port, dlg.baudrate, dlg.parity, dlg.stopbits)
            self._instrument.send_command("SYST:REM")
            self._instrument.send_command(MODES[self._current_mode].conf_cmd)
            self._instrument.start_sampling()
            self._connect_btn.setText("Trennen")
            self._stop_btn.setEnabled(True)
            self._local_btn.setEnabled(True)
            self._status.showMessage(f"Verbunden: {dlg.port} @ {dlg.baudrate}")
        except Exception as e:
            QMessageBox.critical(self, "Verbindungsfehler", str(e))

    def _on_stop_clicked(self):
        self._instrument.stop_sampling()
        self._stop_btn.setEnabled(False)
        self._status.showMessage("Messung gestoppt")

    def _on_local_clicked(self):
        if self._instrument.is_connected:
            self._instrument.send_command("SYST:LOC")

    def closeEvent(self, event):
        self._instrument.stop_sampling()
        if self._instrument.is_connected:
            self._instrument.disconnect()
        event.accept()
