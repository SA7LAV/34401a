#pragma once
#include <QString>
#include <QList>
#include <QMap>
#include <QPair>
#include <utility>

enum class MeasMode {
    VDC, ADC, VAC, AAC, OHM2, OHM4, FREQ, PERIOD, DIODE, CONT
};

// ranges: ordered list of {display label, SCPI command}; empty command = Auto
using RangeList = QList<QPair<QString, QString>>;

struct ModeConfig {
    QString   label;
    QString   confCmd;
    QString   unit;
    RangeList ranges;
};

extern const QMap<MeasMode, ModeConfig> MODES;

// Returns {formatted number string, SI prefix}
std::pair<QString, QString> formatValue(double value, int decimals = 4);
