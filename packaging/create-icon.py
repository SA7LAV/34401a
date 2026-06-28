"""Erzeugt das App-Icon als 128x128 PNG mit PyQt5."""
import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

from PyQt5.QtWidgets import QApplication
from PyQt5.QtGui import QPixmap, QPainter, QColor, QFont, QPen
from PyQt5.QtCore import Qt, QRect

app = QApplication(sys.argv)

px = QPixmap(128, 128)
px.fill(QColor("#0A0A1A"))

p = QPainter(px)
p.setRenderHint(QPainter.Antialiasing)

# Omega-Symbol groß
font = QFont("DejaVu Sans", 62, QFont.Bold)
p.setFont(font)
p.setPen(QColor("#00C0FF"))
p.drawText(QRect(0, 0, 128, 88), Qt.AlignHCenter | Qt.AlignVCenter, "Ω")

# "34401A" unten
font2 = QFont("DejaVu Sans", 13, QFont.Bold)
p.setFont(font2)
p.setPen(QColor("#0080AA"))
p.drawText(QRect(0, 92, 128, 32), Qt.AlignHCenter | Qt.AlignVCenter, "34401A")

p.end()

out = os.path.join(os.path.dirname(__file__), '..', 'assets', 'hp34401a.png')
px.save(out)
print(f"Icon gespeichert: {os.path.abspath(out)}")
