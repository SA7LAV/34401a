from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QLabel, QFrame
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QFont
from models import format_value

DISPLAY_COLOR = "#00C0FF"
SECONDARY_COLOR = "#0080AA"
BG_COLOR = "#0A0A1A"

MAIN_FONT = QFont("Courier New", 64, QFont.Bold)
UNIT_FONT = QFont("Courier New", 40, QFont.Bold)
SECONDARY_FONT = QFont("Courier New", 18)
LABEL_FONT = QFont("Arial", 10)


def _lbl(text, font, color, align=Qt.AlignRight | Qt.AlignVCenter):
    w = QLabel(text)
    w.setFont(font)
    w.setStyleSheet(f"color: {color}; background: transparent;")
    w.setAlignment(align)
    return w


class DisplayWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setStyleSheet(f"background-color: {BG_COLOR};")
        self._unit = "VDC"
        self._stats = {"MIN": None, "MAX": None, "AVG": None, "_count": 0, "_sum": 0.0}
        self._build_ui()

    def _build_ui(self):
        root = QVBoxLayout(self)
        root.setContentsMargins(16, 16, 16, 16)

        main_row = QHBoxLayout()
        self._val_label = _lbl("----", MAIN_FONT, DISPLAY_COLOR)
        self._prefix_label = _lbl("", UNIT_FONT, DISPLAY_COLOR)
        self._unit_label = _lbl("VDC", UNIT_FONT, DISPLAY_COLOR, Qt.AlignLeft | Qt.AlignVCenter)
        main_row.addStretch()
        main_row.addWidget(self._val_label)
        main_row.addWidget(self._prefix_label)
        main_row.addWidget(self._unit_label)
        root.addLayout(main_row)

        line = QFrame()
        line.setFrameShape(QFrame.HLine)
        line.setStyleSheet(f"color: {SECONDARY_COLOR};")
        root.addWidget(line)

        stats_row = QHBoxLayout()
        self._min_lbl = self._stat_block("MIN", stats_row)
        self._max_lbl = self._stat_block("MAX", stats_row)
        self._avg_lbl = self._stat_block("AVG", stats_row)
        root.addLayout(stats_row)

    def _stat_block(self, title, layout):
        col = QVBoxLayout()
        t = QLabel(title)
        t.setFont(LABEL_FONT)
        t.setStyleSheet(f"color: {SECONDARY_COLOR};")
        v = _lbl("----", SECONDARY_FONT, SECONDARY_COLOR)
        col.addWidget(t)
        col.addWidget(v)
        layout.addLayout(col)
        return v

    def update_value(self, raw_value: float) -> None:
        val_str, prefix = format_value(raw_value)
        self._val_label.setText(val_str)
        self._prefix_label.setText(prefix)

        s = self._stats
        s["_count"] += 1
        s["_sum"] += raw_value
        if s["MIN"] is None or raw_value < s["MIN"]:
            s["MIN"] = raw_value
        if s["MAX"] is None or raw_value > s["MAX"]:
            s["MAX"] = raw_value
        s["AVG"] = s["_sum"] / s["_count"]

        def fmt(v):
            vs, p = format_value(v)
            return f"{vs} {p}{self._unit}"

        self._min_lbl.setText(fmt(s["MIN"]))
        self._max_lbl.setText(fmt(s["MAX"]))
        self._avg_lbl.setText(fmt(s["AVG"]))

    def set_unit(self, unit: str) -> None:
        self._unit = unit
        self._unit_label.setText(unit)

    def reset_stats(self) -> None:
        self._stats = {"MIN": None, "MAX": None, "AVG": None, "_count": 0, "_sum": 0.0}
        self._val_label.setText("----")
        self._prefix_label.setText("")
        self._min_lbl.setText("----")
        self._max_lbl.setText("----")
        self._avg_lbl.setText("----")
