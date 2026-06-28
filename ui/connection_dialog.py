import configparser
import os
from PyQt5.QtWidgets import (
    QDialog, QVBoxLayout, QFormLayout, QComboBox, QDialogButtonBox
)
from instrument import Instrument

CONFIG_FILE = os.path.expanduser("~/.config/hp34401a/serial_config.ini")


class ConnectionDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Serielle Verbindung")
        self.setModal(True)
        self._build_ui()
        self._load_config()

    def _build_ui(self):
        layout = QVBoxLayout(self)
        form = QFormLayout()

        self.port_combo = QComboBox()
        self.port_combo.addItems(Instrument.list_ports())
        form.addRow("Port:", self.port_combo)

        self.baud_combo = QComboBox()
        self.baud_combo.addItems(["300", "600", "1200", "2400", "4800",
                                   "9600", "19200", "38400", "57600", "115200"])
        self.baud_combo.setCurrentText("9600")
        form.addRow("Baudrate:", self.baud_combo)

        self.parity_combo = QComboBox()
        self.parity_combo.addItems(["None (N)", "Even (E)", "Odd (O)"])
        form.addRow("Parität:", self.parity_combo)

        self.stopbits_combo = QComboBox()
        self.stopbits_combo.addItems(["1", "1.5", "2"])
        form.addRow("Stoppbits:", self.stopbits_combo)

        layout.addLayout(form)
        buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttons.accepted.connect(self._save_and_accept)
        buttons.rejected.connect(self.reject)
        layout.addWidget(buttons)

    def _parity_char(self) -> str:
        return self.parity_combo.currentText()[0]

    def _save_and_accept(self):
        os.makedirs(os.path.dirname(CONFIG_FILE), exist_ok=True)
        cfg = configparser.ConfigParser()
        cfg["serial"] = {
            "port": self.port_combo.currentText(),
            "baudrate": self.baud_combo.currentText(),
            "parity": self._parity_char(),
            "stopbits": self.stopbits_combo.currentText(),
        }
        with open(CONFIG_FILE, "w") as f:
            cfg.write(f)
        self.accept()

    def _load_config(self):
        if not os.path.exists(CONFIG_FILE):
            return
        cfg = configparser.ConfigParser()
        cfg.read(CONFIG_FILE)
        port = cfg.get("serial", "port", fallback=None)
        ports = [self.port_combo.itemText(i) for i in range(self.port_combo.count())]
        if port and port in ports:
            self.port_combo.setCurrentText(port)
        self.baud_combo.setCurrentText(cfg.get("serial", "baudrate", fallback="9600"))
        parity_map = {"N": "None (N)", "E": "Even (E)", "O": "Odd (O)"}
        self.parity_combo.setCurrentText(
            parity_map.get(cfg.get("serial", "parity", fallback="N"), "None (N)")
        )
        self.stopbits_combo.setCurrentText(cfg.get("serial", "stopbits", fallback="1"))

    @property
    def port(self) -> str:
        return self.port_combo.currentText()

    @property
    def baudrate(self) -> int:
        return int(self.baud_combo.currentText())

    @property
    def parity(self) -> str:
        return self._parity_char()

    @property
    def stopbits(self) -> float:
        return float(self.stopbits_combo.currentText())
