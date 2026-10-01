#include "rangepanel.h"
#include <QHBoxLayout>
#include <QPushButton>

static const char* ACTIVE_SS =
    "QPushButton { background: #00C0FF; color: #000; font-weight: bold; "
    "border-radius: 4px; padding: 4px 8px; }";

static const char* INACTIVE_SS =
    "QPushButton { background: #1A2040; color: #00C0FF; "
    "border: 1px solid #00C0FF; border-radius: 4px; padding: 4px 8px; }"
    "QPushButton:hover { background: #203060; }";

RangePanel::RangePanel(QWidget* parent) : QWidget(parent)
{
    m_layout = new QHBoxLayout(this);
    m_layout->setSpacing(4);
}

void RangePanel::setMode(MeasMode mode)
{
    for (auto* btn : m_buttons) {
        m_layout->removeWidget(btn);
        btn->hide();
        btn->deleteLater();
    }
    m_buttons.clear();
    m_activeBtn = nullptr;

    const auto& ranges = MODES[mode].ranges;
    if (ranges.isEmpty()) return;

    for (const auto& [label, cmd] : ranges) {
        auto* btn = new QPushButton(label);
        btn->setStyleSheet(INACTIVE_SS);
        connect(btn, &QPushButton::clicked, this, [this, cmd, btn]() { onSelect(cmd, btn); });
        m_layout->addWidget(btn);
        m_buttons.append(btn);
    }

    if (!m_buttons.isEmpty()) {
        m_silent = true;
        onSelect("", m_buttons.first()); // select Auto (first button, empty cmd)
        m_silent = false;
    }
}

void RangePanel::onSelect(const QString& cmd, QPushButton* btn)
{
    if (m_activeBtn)
        m_activeBtn->setStyleSheet(INACTIVE_SS);
    m_activeBtn = btn;
    btn->setStyleSheet(ACTIVE_SS);
    if (!m_silent)
        emit rangeSelected(cmd);
}
