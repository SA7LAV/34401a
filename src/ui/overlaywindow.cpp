#include "overlaywindow.h"
#include "../models.h"
#include "../config.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QMouseEvent>
#include <QMoveEvent>
#include <QScreen>
#include <QApplication>
#include <QFont>
#include <QWindow>

OverlayWindow::OverlayWindow(QWidget* parent)
    : QWidget(parent,
              Qt::FramelessWindowHint |
              Qt::WindowStaysOnTopHint |
              Qt::Tool)
{
    m_saveTimer = new QTimer(this);
    m_saveTimer->setSingleShot(true);
    m_saveTimer->setInterval(500);
    connect(m_saveTimer, &QTimer::timeout, this, &OverlayWindow::savePos);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(4);

    auto mkLbl = [](Qt::Alignment align = Qt::AlignRight | Qt::AlignVCenter) -> QLabel* {
        auto* w = new QLabel;
        w->setAlignment(align);
        return w;
    };
    m_valLabel    = mkLbl();
    m_prefixLabel = mkLbl();
    m_unitLabel   = mkLbl(Qt::AlignLeft | Qt::AlignVCenter);

    layout->addWidget(m_valLabel);
    layout->addWidget(m_prefixLabel);
    layout->addWidget(m_unitLabel);

    applyStyle();
    restorePos();
}

void OverlayWindow::restorePos()
{
    QPoint pos = AppConfig::instance().overlayPos();
    if (pos.x() >= 0 && pos.y() >= 0) {
        move(pos);
    } else {
        QRect screen = QApplication::primaryScreen()->availableGeometry();
        move(screen.right() - 400, screen.top() + 20);
    }
}

void OverlayWindow::savePos()
{
    QPoint pos = frameGeometry().topLeft();
    AppConfig::instance().saveOverlayPos(pos.x(), pos.y());
}

void OverlayWindow::moveEvent(QMoveEvent* event)
{
    QWidget::moveEvent(event);
    m_saveTimer->start();
}

void OverlayWindow::applyStyle()
{
    QString color   = AppConfig::instance().overlayColor();
    QString bgColor = AppConfig::instance().overlayBgColor();
    QString family  = AppConfig::instance().overlayFont();
    int size        = AppConfig::instance().overlaySize();

    // Window background (chroma-key colour for OBS); labels stay transparent.
    setStyleSheet(QString("background: %1;").arg(bgColor));

    QFont valFont (family, size,        QFont::Bold);
    QFont unitFont(family, size * 2 / 3, QFont::Bold);
    QString style = QString("color: %1; background: transparent;").arg(color);

    for (auto* lbl : {m_valLabel, m_prefixLabel}) {
        lbl->setFont(valFont);
        lbl->setStyleSheet(style);
    }
    m_unitLabel->setFont(unitFont);
    m_unitLabel->setStyleSheet(style);
}

void OverlayWindow::updateValue(double rawValue)
{
    auto [valStr, prefix] = formatValue(rawValue, AppConfig::instance().decimals());
    m_valLabel->setText(valStr);
    m_prefixLabel->setText(prefix);
    m_prefixLabel->setVisible(!prefix.isEmpty());
    m_unitLabel->setVisible(true); // restore after a possible overload display
}

void OverlayWindow::showOverload(const QString& text)
{
    m_valLabel->setText(text);
    m_prefixLabel->setText("");
    m_prefixLabel->setVisible(false);
    m_unitLabel->setVisible(false);
}

void OverlayWindow::setUnit(const QString& unit)
{
    m_unit = unit;
    m_unitLabel->setText(unit);
    m_unitLabel->setVisible(true); // may have been hidden by a prior overload
}

void OverlayWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // Use native system move if available (Wayland/X11)
        if (QWindow* handle = windowHandle()) {
            handle->startSystemMove();
        } else {
            m_dragPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
        }
        event->accept();
    }
}

void OverlayWindow::mouseMoveEvent(QMouseEvent* event)
{
    if ((event->buttons() & Qt::LeftButton) && !m_dragPos.isNull()) {
        move(event->globalPosition().toPoint() - m_dragPos);
        event->accept();
    }
}

void OverlayWindow::mouseReleaseEvent(QMouseEvent* event)
{
    m_dragPos = QPoint();
    event->accept();
}
