/**
 * @file models.cpp
 * @brief MODES table and formatValue() implementation.
 */
#include "models.h"
#include <cmath>

// Convenience macro for building RangeList entries; undefined after MODES.
#define R(label, cmd) QPair<QString,QString>{label, cmd}

const QMap<MeasMode, ModeConfig> MODES = {
    // The 34401A has no :RANG subsystem: the range is the first parameter
    // of the CONFigure command (see the 34401A Programming Guide, "The
    // MEASure? and CONFigure Commands").  Entries hold the numeric range
    // value; buildConfCommand() combines it with the configured resolution.
    {MeasMode::VDC, {"VDC", "CONF:VOLT:DC DEF,DEF", "VDC", {
        R("Auto",   ""),
        R("100 mV", "0.1"),
        R("1 V",    "1"),
        R("10 V",   "10"),
        R("100 V",  "100"),
        R("1000 V", "1000"),
    }}},
    {MeasMode::ADC, {"ADC", "CONF:CURR:DC DEF,DEF", "ADC", {
        R("Auto",   ""),
        R("100 mA", "0.1"),
        R("1 A",    "1"),
        R("3 A",    "3"),
    }}},
    {MeasMode::VAC, {"VAC", "CONF:VOLT:AC DEF,DEF", "VAC", {
        R("Auto",   ""),
        R("100 mV", "0.1"),
        R("1 V",    "1"),
        R("10 V",   "10"),
        R("100 V",  "100"),
        R("750 V",  "750"),
    }}},
    {MeasMode::AAC, {"AAC", "CONF:CURR:AC DEF,DEF", "AAC", {
        R("Auto", ""),
        R("1 A",  "1"),
        R("3 A",  "3"),
    }}},
    {MeasMode::OHM2, {u8"2W Ω", "CONF:RES DEF,DEF", u8"Ω", {
        R("Auto",     ""),
        R(u8"100 Ω",  "100"),
        R(u8"1 kΩ",   "1000"),
        R(u8"10 kΩ",  "10000"),
        R(u8"100 kΩ", "100000"),
        R(u8"1 MΩ",   "1000000"),
        R(u8"10 MΩ",  "10000000"),
        R(u8"100 MΩ", "100000000"),
    }}},
    {MeasMode::OHM4, {u8"4W Ω", "CONF:FRES DEF,DEF", u8"Ω", {
        R("Auto",     ""),
        R(u8"100 Ω",  "100"),
        R(u8"1 kΩ",   "1000"),
        R(u8"10 kΩ",  "10000"),
        R(u8"100 kΩ", "100000"),
        R(u8"1 MΩ",   "1000000"),
        R(u8"10 MΩ",  "10000000"),
        R(u8"100 MΩ", "100000000"),
    }}},
    // Frequency and period use a single fixed range (3 Hz to 300 kHz);
    // there is no range selection for these functions.
    {MeasMode::FREQ,   {"FREQ", "CONF:FREQ DEF,DEF", "Hz", {}}},
    {MeasMode::PERIOD, {"PERIOD", "CONF:PER DEF,DEF", "s", {}}},
    {MeasMode::DIODE, {"DIODE", "CONF:DIOD", "VDC", {}}},
    {MeasMode::CONT,  {"CONT",  "CONF:CONT", u8"Ω", {}}},
};

#undef R

// SI prefix table ordered from largest to smallest.  The threshold == 1e-12
// entry acts as the catch-all: any value smaller than 1 pico- (or exactly 0)
// matches here and is displayed with the "p" prefix.
static const struct { double threshold; const char* prefix; } SI_PREFIXES[] = {
    {1e12,  "T"}, {1e9,  "G"}, {1e6,  "M"}, {1e3,  "k"},
    {1.0,   ""},  {1e-3, "m"}, {1e-6, u8"µ"}, {1e-9, "n"}, {1e-12, "p"},
};

QString buildConfCommand(MeasMode mode, const QString& rangeValue, int counts)
{
    const ModeConfig& cfg = MODES[mode];
    switch (mode) {
    case MeasMode::DIODE:  return "CONF:DIOD";
    case MeasMode::CONT:   return "CONF:CONT";
    case MeasMode::FREQ:   return "CONF:FREQ DEF,DEF";
    case MeasMode::PERIOD: return "CONF:PER DEF,DEF";
    default:
        break;
    }
    const QString func = cfg.confCmd.section(' ', 0, 0); // e.g. "CONF:VOLT:DC"
    if (rangeValue.isEmpty())
        return func + " DEF,DEF";
    // Resolution in measurement units per count: range / counts.
    double res = rangeValue.toDouble() / counts;
    return QString("%1 %2,%3").arg(func, rangeValue, QString::number(res, 'e', 0));
}

std::pair<QString, QString> formatValue(double value, int decimals)
{
    double absVal = std::abs(value);
    for (auto& [threshold, prefix] : SI_PREFIXES) {
        if (absVal >= threshold || threshold == 1e-12) {
            double scaled = value / threshold;
            return {QString::number(scaled, 'f', decimals), QString::fromUtf8(prefix)};
        }
    }
    return {QString::number(value, 'e', 6), ""};
}
