/**
 * @file displaywidget.h
 * @brief Large numeric readout with MIN/MAX/AVG statistics strip.
 *
 * The widget has two zones:
 *  1. **Main readout** – a large, monospaced display showing the current
 *     measurement value, an optional SI prefix (m, k, M, …), and the unit
 *     string (e.g. "VDC").  The SI prefix label is hidden when the value
 *     has no prefix, so the unit sits flush against the number.
 *  2. **Statistics strip** – a compact row showing the running MIN, MAX, and
 *     AVG since the last resetStats() call.  Visibility is controlled by the
 *     Display → "Show statistics" setting.
 */
#pragma once
#include <QWidget>
#include <QLabel>
#include <QBoxLayout>
#include <QString>
#include <optional>

/**
 * @brief Passive display widget; all updates arrive via updateValue().
 *
 * Statistics accumulate from the moment a connection is established.
 * MainWindow calls resetStats() on every mode change to restart the
 * MIN/MAX/AVG calculation.
 */
class DisplayWidget : public QWidget {
    Q_OBJECT
public:
    explicit DisplayWidget(QWidget* parent = nullptr);

    /** @brief Sets the unit string appended to every reading (e.g. "VDC", "kΩ"). */
    void setUnit(const QString& unit);

    /**
     * @brief Resets all statistics and blanks the display to "----".
     *
     * Called by MainWindow whenever the measurement mode changes.
     */
    void resetStats();

    /**
     * @brief Re-reads the show_stats config flag and updates strip visibility.
     *
     * Called after the Setup dialog is closed in case the setting changed.
     */
    void applyDisplayConfig();

public slots:
    /**
     * @brief Updates the main readout and accumulates MIN/MAX/AVG statistics.
     *
     * @param rawValue  Numeric value as returned by the instrument (SI-scaled
     *                  by formatValue() before display).
     *
     * Statistics are always accumulated regardless of whether the strip is
     * currently visible.
     */
    void updateValue(double rawValue);

    /**
     * @brief Shows an overload indicator instead of a numeric value.
     *
     * Called when the instrument reports an overrange condition. The main
     * readout is replaced by @p text (e.g. "OVL.D" or "OPEN") and the unit
     * label is hidden; statistics are left untouched. The next call to
     * updateValue() restores normal numeric display.
     *
     * @param text  Overload text to display (already localised/mode-specific).
     */
    void showOverload(const QString& text);

private:
    /** @brief Constructs the two-zone layout. */
    void buildUi();

    /**
     * @brief Creates a labelled value column for the statistics strip.
     *
     * @param title   Label text shown above the value (e.g. "MIN").
     * @param layout  Parent layout to which the column is added.
     * @return Pointer to the value QLabel (owned by the widget tree).
     */
    QLabel* statBlock(const QString& title, QBoxLayout* layout);

    QLabel*  m_valLabel{nullptr};    ///< Current value (large font)
    QLabel*  m_prefixLabel{nullptr}; ///< SI prefix; hidden when empty
    QLabel*  m_unitLabel{nullptr};   ///< Unit string (smaller font)
    QWidget* m_statsFrame{nullptr};  ///< Container for the statistics strip
    QLabel*  m_minLabel{nullptr};
    QLabel*  m_maxLabel{nullptr};
    QLabel*  m_avgLabel{nullptr};

    QString  m_unit{"VDC"};
    std::optional<double> m_min;
    std::optional<double> m_max;
    double   m_sum{0.0};
    int      m_count{0};
};
