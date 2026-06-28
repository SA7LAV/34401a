from PyQt5.QtWidgets import (
    QDialog, QVBoxLayout, QFormLayout, QComboBox, QDialogButtonBox, QLabel
)
from translations import tr
from config import app_config
from instrument import Instrument
from ui import DIALOG_STYLE


class ConnectionDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setModal(True)
        self.setStyleSheet(DIALOG_STYLE)
        self._build_ui()
        self._load_config()
        self.setWindowTitle(tr("dlg_conn_title"))

    def _build_ui(self):
        layout = QVBoxLayout(self)
        form = QFormLayout()

        self._port_lbl = QLabel(tr("lbl_port"))
        self.port_combo = QComboBox()
        self.port_combo.addItems(Instrument.list_ports())
        form.addRow(self._port_lbl, self.port_combo)

        self._baud_lbl = QLabel(tr("lbl_baudrate"))
        self.baud_combo = QComboBox()
        self.baud_combo.addItems(["300", "600", "1200", "2400", "4800",
                                   "9600", "19200", "38400", "57600", "115200"])
        self.baud_combo.setCurrentText("9600")
        form.addRow(self._baud_lbl, self.baud_combo)

        self._parity_lbl = QLabel(tr("lbl_parity"))
        self.parity_combo = QComboBox()
        self.parity_combo.addItem(tr("parity_none"), "N")
        self.parity_combo.addItem(tr("parity_even"), "E")
        self.parity_combo.addItem(tr("parity_odd"),  "O")
        form.addRow(self._parity_lbl, self.parity_combo)

        self._stopbits_lbl = QLabel(tr("lbl_stopbits"))
        self.stopbits_combo = QComboBox()
        self.stopbits_combo.addItems(["1", "1.5", "2"])
        form.addRow(self._stopbits_lbl, self.stopbits_combo)

        self._bytesize_lbl = QLabel(tr("lbl_bytesize"))
        self.bytesize_combo = QComboBox()
        self.bytesize_combo.addItems(["7", "8"])
        form.addRow(self._bytesize_lbl, self.bytesize_combo)

        self._timeout_lbl = QLabel(tr("lbl_timeout"))
        self.timeout_combo = QComboBox()
        self.timeout_combo.addItems(["1.0", "2.0", "5.0", "9.0", "15.0"])
        form.addRow(self._timeout_lbl, self.timeout_combo)

        layout.addLayout(form)
        buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttons.accepted.connect(self._save_and_accept)
        buttons.rejected.connect(self.reject)
        layout.addWidget(buttons)

    def _save_and_accept(self):
        app_config.set("serial", "port",     self.port_combo.currentText())
        app_config.set("serial", "baudrate", self.baud_combo.currentText())
        app_config.set("serial", "parity",   self.parity_combo.currentData())
        app_config.set("serial", "stopbits", self.stopbits_combo.currentText())
        app_config.set("serial", "bytesize", self.bytesize_combo.currentText())
        app_config.set("serial", "timeout",  self.timeout_combo.currentText())
        app_config.save()
        self.accept()

    def _load_config(self):
        ports = [self.port_combo.itemText(i) for i in range(self.port_combo.count())]
        if app_config.serial_port in ports:
            self.port_combo.setCurrentText(app_config.serial_port)
        self.baud_combo.setCurrentText(app_config.serial_baudrate)
        parity_idx = self.parity_combo.findData(app_config.serial_parity)
        self.parity_combo.setCurrentIndex(max(0, parity_idx))
        self.stopbits_combo.setCurrentText(app_config.serial_stopbits)
        self.bytesize_combo.setCurrentText(app_config.serial_bytesize)
        self.timeout_combo.setCurrentText(app_config.serial_timeout)

    def retranslate_ui(self) -> None:
        self.setWindowTitle(tr("dlg_conn_title"))
        self._port_lbl.setText(tr("lbl_port"))
        self._baud_lbl.setText(tr("lbl_baudrate"))
        self._parity_lbl.setText(tr("lbl_parity"))
        self._stopbits_lbl.setText(tr("lbl_stopbits"))
        self._bytesize_lbl.setText(tr("lbl_bytesize"))
        self._timeout_lbl.setText(tr("lbl_timeout"))
        current_parity = self.parity_combo.currentData()
        self.parity_combo.setItemText(0, tr("parity_none"))
        self.parity_combo.setItemText(1, tr("parity_even"))
        self.parity_combo.setItemText(2, tr("parity_odd"))
        idx = self.parity_combo.findData(current_parity)
        self.parity_combo.setCurrentIndex(max(0, idx))

    @property
    def port(self) -> str:
        return self.port_combo.currentText()

    @property
    def baudrate(self) -> int:
        return int(self.baud_combo.currentText())

    @property
    def parity(self) -> str:
        return self.parity_combo.currentData()

    @property
    def stopbits(self) -> float:
        return float(self.stopbits_combo.currentText())

    @property
    def bytesize(self) -> int:
        return int(self.bytesize_combo.currentText())

    @property
    def timeout(self) -> float:
        return float(self.timeout_combo.currentText())
