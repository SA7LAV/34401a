from PyQt5.QtWidgets import QWidget, QHBoxLayout, QPushButton
from PyQt5.QtCore import pyqtSignal
from models import MeasMode, MODES

ACTIVE = ("QPushButton { background: #00C0FF; color: #000; font-weight: bold; "
          "border-radius: 4px; padding: 4px 8px; }")
INACTIVE = ("QPushButton { background: #1A2040; color: #00C0FF; "
            "border: 1px solid #00C0FF; border-radius: 4px; padding: 4px 8px; }"
            "QPushButton:hover { background: #203060; }")


class RangePanel(QWidget):
    range_selected = pyqtSignal(object)  # SCPI-Befehl (str) oder None für Auto

    def __init__(self, parent=None):
        super().__init__(parent)
        self._buttons = []
        self._active_btn = None
        self._layout = QHBoxLayout(self)
        self._layout.setSpacing(4)

    def set_mode(self, mode: MeasMode) -> None:
        for btn in self._buttons:
            self._layout.removeWidget(btn)
            btn.deleteLater()
        self._buttons.clear()
        self._active_btn = None

        ranges = MODES[mode].ranges
        if not ranges:
            return

        for label, scpi_cmd in ranges.items():
            btn = QPushButton(label)
            btn.setStyleSheet(INACTIVE)
            btn.clicked.connect(
                lambda checked, cmd=scpi_cmd, b=btn: self._select(cmd, b)
            )
            self._layout.addWidget(btn)
            self._buttons.append(btn)

        if self._buttons:
            self._select(None, self._buttons[0])

    def _select(self, cmd, btn):
        if self._active_btn:
            self._active_btn.setStyleSheet(INACTIVE)
        self._active_btn = btn
        btn.setStyleSheet(ACTIVE)
        self.range_selected.emit(cmd)
