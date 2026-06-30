#include "models.h"
#include <cmath>

#define R(label, cmd) QPair<QString,QString>{label, cmd}

const QMap<MeasMode, ModeConfig> MODES = {
    {MeasMode::VDC, {"VDC", "CONF:VOLT:DC DEF,DEF", "VDC", {
        R("Auto",   ""),
        R("100 mV", "VOLT:DC:RANG 0.1"),
        R("1 V",    "VOLT:DC:RANG 1"),
        R("10 V",   "VOLT:DC:RANG 10"),
        R("100 V",  "VOLT:DC:RANG 100"),
        R("1000 V", "VOLT:DC:RANG 1000"),
    }}},
    {MeasMode::ADC, {"ADC", "CONF:CURR:DC DEF,DEF", "ADC", {
        R("Auto",   ""),
        R("100 mA", "CURR:DC:RANG 0.1"),
        R("1 A",    "CURR:DC:RANG 1"),
        R("3 A",    "CURR:DC:RANG 3"),
    }}},
    {MeasMode::VAC, {"VAC", "CONF:VOLT:AC DEF,DEF", "VAC", {
        R("Auto",   ""),
        R("100 mV", "VOLT:AC:RANG 0.1"),
        R("1 V",    "VOLT:AC:RANG 1"),
        R("10 V",   "VOLT:AC:RANG 10"),
        R("100 V",  "VOLT:AC:RANG 100"),
        R("1000 V", "VOLT:AC:RANG 1000"),
    }}},
    {MeasMode::AAC, {"AAC", "CONF:CURR:AC DEF,DEF", "AAC", {
        R("Auto", ""),
        R("1 A",  "CURR:AC:RANG 1"),
        R("3 A",  "CURR:AC:RANG 3"),
    }}},
    {MeasMode::OHM2, {u8"2W Ω", "CONF:RES DEF,DEF", u8"Ω", {
        R("Auto",     ""),
        R(u8"100 Ω",  "RES:RANG 100"),
        R(u8"1 kΩ",   "RES:RANG 1000"),
        R(u8"10 kΩ",  "RES:RANG 10000"),
        R(u8"100 kΩ", "RES:RANG 100000"),
        R(u8"1 MΩ",   "RES:RANG 1000000"),
        R(u8"10 MΩ",  "RES:RANG 10000000"),
        R(u8"100 MΩ", "RES:RANG 100000000"),
    }}},
    {MeasMode::OHM4, {u8"4W Ω", "CONF:FRES DEF,DEF", u8"Ω", {
        R("Auto",     ""),
        R(u8"100 Ω",  "FRES:RANG 100"),
        R(u8"1 kΩ",   "FRES:RANG 1000"),
        R(u8"10 kΩ",  "FRES:RANG 10000"),
        R(u8"100 kΩ", "FRES:RANG 100000"),
        R(u8"1 MΩ",   "FRES:RANG 1000000"),
        R(u8"10 MΩ",  "FRES:RANG 10000000"),
        R(u8"100 MΩ", "FRES:RANG 100000000"),
    }}},
    {MeasMode::FREQ, {"FREQ", "CONF:FREQ DEF,DEF", "Hz", {
        R("Auto",   ""),
        R("100 mV", "FREQ:VOLT:RANG 0.1"),
        R("1 V",    "FREQ:VOLT:RANG 1"),
        R("10 V",   "FREQ:VOLT:RANG 10"),
        R("100 V",  "FREQ:VOLT:RANG 100"),
        R("1000 V", "FREQ:VOLT:RANG 1000"),
    }}},
    {MeasMode::PERIOD, {"PERIOD", "CONF:PER DEF,DEF", "s", {
        R("Auto",   ""),
        R("100 mV", "PER:VOLT:RANG 0.1"),
        R("1 V",    "PER:VOLT:RANG 1"),
        R("10 V",   "PER:VOLT:RANG 10"),
        R("100 V",  "PER:VOLT:RANG 100"),
        R("1000 V", "PER:VOLT:RANG 1000"),
    }}},
    {MeasMode::DIODE, {"DIODE", "CONF:DIOD", "VDC", {}}},
    {MeasMode::CONT,  {"CONT",  "CONF:CONT", u8"Ω", {}}},
};

#undef R

static const struct { double threshold; const char* prefix; } SI_PREFIXES[] = {
    {1e12,  "T"}, {1e9,  "G"}, {1e6,  "M"}, {1e3,  "k"},
    {1.0,   ""},  {1e-3, "m"}, {1e-6, u8"µ"}, {1e-9, "n"}, {1e-12, "p"},
};

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
