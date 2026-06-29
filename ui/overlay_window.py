from PyQt5.QtWidgets import QWidget, QHBoxLayout, QLabel
from PyQt5.QtCore import Qt, QPoint
from PyQt5.QtGui import QFont
from models import format_value
from config import app_config

_VAL_FONT  = QFont("Courier New", 72, QFont.Bold)
_UNIT_FONT = QFont("Courier New", 48, QFont.Bold)
_COLOR     = "#00C0FF"


class OverlayWindow(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent,
                         Qt.FramelessWindowHint |
                         Qt.WindowStaysOnTopHint |
                         Qt.Tool)
        self.setStyleSheet("background: #00FF00;")
        self._drag_pos = QPoint()
        self._unit = "VDC"
        self._build_ui()

    def _build_ui(self):
        layout = QHBoxLayout(self)
        layout.setContentsMargins(12, 8, 12, 8)

        def lbl(text, font, align=Qt.AlignRight | Qt.AlignVCenter):
            w = QLabel(text)
            w.setFont(font)
            w.setStyleSheet(f"color: {_COLOR}; background: transparent;")
            w.setAlignment(align)
            return w

        self._val_label    = lbl("----", _VAL_FONT)
        self._prefix_label = lbl("",     _UNIT_FONT)
        self._unit_label   = lbl("VDC",  _UNIT_FONT, Qt.AlignLeft | Qt.AlignVCenter)

        for label in (self._val_label, self._prefix_label, self._unit_label):
            label.setAttribute(Qt.WA_TransparentForMouseEvents, True)

        layout.addWidget(self._val_label)
        layout.addWidget(self._prefix_label)
        layout.addWidget(self._unit_label)

    def update_value(self, raw_value: float) -> None:
        val_str, prefix = format_value(raw_value, decimals=app_config.decimals)
        self._val_label.setText(val_str)
        self._prefix_label.setText(prefix)

    def set_unit(self, unit: str) -> None:
        self._unit = unit
        self._unit_label.setText(unit)

    # drag support (kein Fensterrahmen)
    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            self._drag_pos = event.globalPos() - self.frameGeometry().topLeft()
            event.accept()

    def mouseMoveEvent(self, event):
        if event.buttons() & Qt.LeftButton and not self._drag_pos.isNull():
            self.move(event.globalPos() - self._drag_pos)
            event.accept()

    def mouseReleaseEvent(self, event):
        self._drag_pos = QPoint()
        event.accept()
