#include "modepanel.h"
#include <QGridLayout>
#include <QPushButton>

static const char* ACTIVE_SS =
    "QPushButton { background: #00C0FF; color: #000; font-weight: bold; "
    "border-radius: 4px; padding: 6px; }";

static const char* INACTIVE_SS =
    "QPushButton { background: #1A2040; color: #00C0FF; font-weight: bold; "
    "border: 1px solid #00C0FF; border-radius: 4px; padding: 6px; }"
    "QPushButton:hover { background: #203060; }";

static const QList<MeasMode> MODE_ORDER = {
    MeasMode::VDC,  MeasMode::ADC,
    MeasMode::VAC,  MeasMode::AAC,
    MeasMode::OHM2, MeasMode::OHM4,
    MeasMode::FREQ, MeasMode::PERIOD,
    MeasMode::DIODE,MeasMode::CONT,
};

ModePanel::ModePanel(QWidget* parent) : QWidget(parent)
{
    auto* grid = new QGridLayout(this);
    grid->setSpacing(6);
    for (int i = 0; i < MODE_ORDER.size(); ++i) {
        MeasMode mode = MODE_ORDER[i];
        auto* btn = new QPushButton(MODES[mode].label);
        btn->setStyleSheet(INACTIVE_SS);
        connect(btn, &QPushButton::clicked, this, [this, mode]() { onButtonClicked(mode); });
        m_buttons[mode] = btn;
        grid->addWidget(btn, i / 2, i % 2);
    }
}

void ModePanel::onButtonClicked(MeasMode mode)
{
    m_buttons[m_active]->setStyleSheet(INACTIVE_SS);
    m_active = mode;
    m_buttons[mode]->setStyleSheet(ACTIVE_SS);
    emit modeChanged(mode);
}

void ModePanel::select(MeasMode mode)
{
    onButtonClicked(mode);
}
