/**
 * @file rangepanel.h
 * @brief Horizontal strip of range-selection buttons for the active mode.
 *
 * The button set is rebuilt from scratch each time setMode() is called,
 * because the available ranges differ per measurement function.  Modes
 * without manual ranging (DIODE, CONT) leave the panel empty.
 *
 * The first button in each set is always "Auto", which causes the instrument
 * to select the optimal range automatically.
 */
#pragma once
#include <QWidget>
#include <QList>
#include "../models.h"

class QPushButton;
class QHBoxLayout;

/**
 * @brief Dynamic row of range buttons that rebuilds itself on mode change.
 *
 * Emits rangeSelected() with the SCPI range-setting command string when the
 * user clicks a button, or an empty string for the Auto range.
 */
class RangePanel : public QWidget {
    Q_OBJECT
public:
    explicit RangePanel(QWidget* parent = nullptr);

    /**
     * @brief Replaces the button set with the ranges for @p mode.
     *
     * All existing range buttons are deleted and new ones are created from
     * ModeConfig::ranges.  The Auto button is always selected after a mode
     * change.  If the mode has no ranges (DIODE, CONT) the panel is left
     * empty and no signal is emitted.
     */
    void setMode(MeasMode mode);

signals:
    /**
     * @brief Emitted when the user selects a range.
     *
     * @param scpiCmd  The SCPI command to set the range
     *                 (e.g. "VOLT:DC:RANG 10"), or an empty string for Auto.
     */
    void rangeSelected(const QString& scpiCmd);

private:
    /**
     * @brief Activates @p btn and emits rangeSelected() with @p cmd.
     *
     * Deactivates the previously active button first.
     */
    void onSelect(const QString& cmd, QPushButton* btn);

    QHBoxLayout*        m_layout{nullptr};
    QList<QPushButton*> m_buttons;
    QPushButton*        m_activeBtn{nullptr}; ///< Currently highlighted button; nullptr before first selection
};
