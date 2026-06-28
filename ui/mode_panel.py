from PyQt5.QtWidgets import QWidget, QGridLayout, QPushButton
from PyQt5.QtCore import pyqtSignal
from models import MeasMode, MODES

MODE_ORDER = [
    MeasMode.VDC,  MeasMode.ADC,
    MeasMode.VAC,  MeasMode.AAC,
    MeasMode.OHM2, MeasMode.OHM4,
    MeasMode.FREQ, MeasMode.PERIOD,
    MeasMode.DIODE, MeasMode.CONT,
]

ACTIVE = ("QPushButton { background: #00C0FF; color: #000; font-weight: bold; "
          "border-radius: 4px; padding: 6px; }")
INACTIVE = ("QPushButton { background: #1A2040; color: #00C0FF; font-weight: bold; "
            "border: 1px solid #00C0FF; border-radius: 4px; padding: 6px; }"
            "QPushButton:hover { background: #203060; }")


class ModePanel(QWidget):
    mode_changed = pyqtSignal(object)

    def __init__(self, parent=None):
        super().__init__(parent)
        self._active = None
        self._buttons = {}
        grid = QGridLayout(self)
        grid.setSpacing(6)
        for i, mode in enumerate(MODE_ORDER):
            btn = QPushButton(MODES[mode].label)
            btn.setStyleSheet(INACTIVE)
            btn.clicked.connect(lambda checked, m=mode: self._select(m))
            self._buttons[mode] = btn
            grid.addWidget(btn, i // 2, i % 2)

    def _select(self, mode):
        if self._active:
            self._buttons[self._active].setStyleSheet(INACTIVE)
        self._active = mode
        self._buttons[mode].setStyleSheet(ACTIVE)
        self.mode_changed.emit(mode)

    def select(self, mode):
        self._select(mode)
