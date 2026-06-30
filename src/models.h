/**
 * @file models.h
 * @brief Domain model types for the 34401A: measurement modes, per-mode
 *        configuration, and value formatting.
 */
#pragma once
#include <QString>
#include <QList>
#include <QMap>
#include <QPair>
#include <utility>

/**
 * @brief Identifies one of the ten measurement functions supported by the
 *        HP/Agilent/Keysight 34401A.
 *
 * The values map directly to SCPI CONF: commands via the MODES table.
 * DIODE and CONT do not support manual range selection; their @c ranges
 * field in ModeConfig is empty.
 */
enum class MeasMode {
    VDC,    ///< DC voltage
    ADC,    ///< DC current
    VAC,    ///< AC voltage (true RMS)
    AAC,    ///< AC current (true RMS)
    OHM2,   ///< 2-wire resistance
    OHM4,   ///< 4-wire resistance (Kelvin sensing)
    FREQ,   ///< Frequency
    PERIOD, ///< Period
    DIODE,  ///< Diode test (no manual ranging)
    CONT    ///< Continuity (no manual ranging)
};

/**
 * @brief Ordered list of available measurement ranges for one mode.
 *
 * Each entry is a {display label, SCPI range command} pair.
 * An empty SCPI command string signals the "Auto" range, which causes the
 * instrument to select the range automatically.
 */
using RangeList = QList<QPair<QString, QString>>;

/**
 * @brief Static configuration record for a single measurement mode.
 */
struct ModeConfig {
    QString   label;    ///< Button label shown in the UI (e.g. "VDC")
    QString   confCmd;  ///< SCPI CONF: command that selects this mode (e.g. "CONF:VOLT:DC")
    QString   unit;     ///< Unit string appended to readings (e.g. "VDC", "kΩ")
    RangeList ranges;   ///< Available manual ranges; empty for DIODE and CONT
};

/**
 * @brief Master table mapping every MeasMode to its ModeConfig.
 *
 * This is the single source of truth for mode metadata used by ModePanel,
 * RangePanel, and MainWindow.
 */
extern const QMap<MeasMode, ModeConfig> MODES;

/**
 * @brief Scales @p value with an appropriate SI prefix and formats it as a
 *        fixed-point string.
 *
 * @param value    Raw double value as returned by the instrument.
 * @param decimals Number of decimal places in the formatted string (default 4).
 * @return A pair of {number string, SI prefix character} where the prefix is
 *         one of "T", "G", "M", "k", "" (none), "m", "µ", "n", or "p".
 *         Values outside the pico–tera range fall back to scientific notation.
 */
std::pair<QString, QString> formatValue(double value, int decimals = 4);
