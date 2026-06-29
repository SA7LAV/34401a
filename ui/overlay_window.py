from PyQt5.QtWidgets import QWidget, QHBoxLayout, QLabel
from PyQt5.QtCore import Qt, QPoint
from PyQt5.QtGui import QFont
from models import format_value
from config import app_config


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
        self.apply_style()

    def _build_ui(self):
        layout = QHBoxLayout(self)
        layout.setContentsMargins(12, 8, 12, 8)

        def lbl(text, align=Qt.AlignRight | Qt.AlignVCenter):
            w = QLabel(text)
            w.setAlignment(align)
            w.setAttribute(Qt.WA_TransparentForMouseEvents, True)
            return w

        self._val_label    = lbl("----")
        self._prefix_label = lbl("")
        self._unit_label   = lbl("VDC", Qt.AlignLeft | Qt.AlignVCenter)

        layout.addWidget(self._val_label)
        layout.addWidget(self._prefix_label)
        layout.addWidget(self._unit_label)

    def apply_style(self) -> None:
        color    = app_config.overlay_color
        family   = app_config.overlay_font
        size     = app_config.overlay_size
        val_font  = QFont(family, size,        QFont.Bold)
        unit_font = QFont(family, size * 2 // 3, QFont.Bold)
        style = f"color: {color}; background: transparent;"
        for label, font in (
            (self._val_label,    val_font),
            (self._prefix_label, unit_font),
            (self._unit_label,   unit_font),
        ):
            label.setFont(font)
            label.setStyleSheet(style)

    def update_value(self, raw_value: float) -> None:
        val_str, prefix = format_value(raw_value, decimals=app_config.decimals)
        self._val_label.setText(val_str)
        self._prefix_label.setText(prefix)

    def set_unit(self, unit: str) -> None:
        self._unit = unit
        self._unit_label.setText(unit)

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
