#include "displaywidget.h"
#include "../models.h"
#include "../config.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QFrame>
#include <QFont>

static const char* BG_COLOR       = "#0A0A1A";
static const char* DISPLAY_COLOR  = "#00C0FF";
static const char* SECONDARY_COLOR= "#0080AA";

static QLabel* makeLabel(const QString& text, const QFont& font, const char* color,
                         Qt::Alignment align = Qt::AlignRight | Qt::AlignVCenter)
{
    auto* w = new QLabel(text);
    w->setFont(font);
    w->setStyleSheet(QString("color: %1; background: transparent;").arg(color));
    w->setAlignment(align);
    return w;
}

DisplayWidget::DisplayWidget(QWidget* parent)
    : QWidget(parent)
{
    setStyleSheet(QString("background-color: %1;").arg(BG_COLOR));
    buildUi();
    applyDisplayConfig();
}

void DisplayWidget::buildUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);

    QFont mainFont("Courier New", 64, QFont::Bold);
    QFont unitFont("Courier New", 40, QFont::Bold);
    QFont secFont ("Courier New", 18);
    QFont lblFont ("Arial", 10);

    auto* mainRow = new QHBoxLayout;
    m_valLabel    = makeLabel("----", mainFont, DISPLAY_COLOR);
    m_prefixLabel = makeLabel("",     unitFont, DISPLAY_COLOR);
    m_unitLabel   = makeLabel("VDC",  unitFont, DISPLAY_COLOR,
                              Qt::AlignLeft | Qt::AlignVCenter);
    mainRow->addStretch();
    mainRow->addWidget(m_valLabel);
    mainRow->addWidget(m_prefixLabel);
    mainRow->addWidget(m_unitLabel);
    root->addLayout(mainRow);

    m_statsFrame = new QWidget;
    auto* statsLayout = new QVBoxLayout(m_statsFrame);
    statsLayout->setContentsMargins(0, 0, 0, 0);

    auto* line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet(QString("color: %1;").arg(SECONDARY_COLOR));
    statsLayout->addWidget(line);

    auto* statsRow = new QHBoxLayout;
    m_minLabel = statBlock("MIN", statsRow);
    m_maxLabel = statBlock("MAX", statsRow);
    m_avgLabel = statBlock("AVG", statsRow);
    statsLayout->addLayout(statsRow);

    root->addWidget(m_statsFrame);

    Q_UNUSED(secFont)
    Q_UNUSED(lblFont)
}

QLabel* DisplayWidget::statBlock(const QString& title, QBoxLayout* layout)
{
    QFont lblFont("Arial", 10);
    QFont secFont("Courier New", 18);

    auto* col = new QVBoxLayout;
    auto* t = new QLabel(title);
    t->setFont(lblFont);
    t->setStyleSheet(QString("color: %1;").arg(SECONDARY_COLOR));
    auto* v = makeLabel("----", secFont, SECONDARY_COLOR);
    col->addWidget(t);
    col->addWidget(v);
    layout->addLayout(col);
    return v;
}

void DisplayWidget::applyDisplayConfig()
{
    m_statsFrame->setVisible(AppConfig::instance().showStats());
}

void DisplayWidget::updateValue(double rawValue)
{
    auto [valStr, prefix] = formatValue(rawValue, AppConfig::instance().decimals());
    m_valLabel->setText(valStr);
    m_prefixLabel->setText(prefix);

    m_count++;
    m_sum += rawValue;
    if (!m_min || rawValue < *m_min) m_min = rawValue;
    if (!m_max || rawValue > *m_max) m_max = rawValue;
    double avg = m_sum / m_count;

    auto fmt = [&](double v) -> QString {
        auto [vs, p] = formatValue(v, AppConfig::instance().decimals());
        return vs + " " + p + m_unit;
    };
    m_minLabel->setText(fmt(*m_min));
    m_maxLabel->setText(fmt(*m_max));
    m_avgLabel->setText(fmt(avg));
}

void DisplayWidget::setUnit(const QString& unit)
{
    m_unit = unit;
    m_unitLabel->setText(unit);
}

void DisplayWidget::resetStats()
{
    m_min.reset();
    m_max.reset();
    m_sum   = 0.0;
    m_count = 0;
    m_valLabel->setText("----");
    m_prefixLabel->setText("");
    m_minLabel->setText("----");
    m_maxLabel->setText("----");
    m_avgLabel->setText("----");
}
