/**
 * @file modepanel.h
 * @brief Grid of toggle buttons for selecting the active measurement function.
 *
 * Buttons are arranged in two columns in the order defined by MODE_ORDER in
 * modepanel.cpp.  Exactly one button is active (highlighted) at any time.
 * Clicking a button calls select() internally and emits modeChanged().
 */
#pragma once
#include <QWidget>
#include <QMap>
#include "../models.h"

class QPushButton;

/**
 * @brief Two-column button grid that tracks and emits the active MeasMode.
 *
 * The initial selection is MeasMode::VDC; call select() to set the active
 * mode programmatically (e.g. on application startup).
 */
class ModePanel : public QWidget {
    Q_OBJECT
public:
    explicit ModePanel(QWidget* parent = nullptr);

    /**
     * @brief Activates @p mode, updating button styles and emitting modeChanged().
     *
     * Safe to call at any time, including before the widget is shown.
     */
    void select(MeasMode mode);

signals:
    /** @brief Emitted whenever the active measurement mode changes. */
    void modeChanged(MeasMode mode);

private:
    /**
     * @brief Handles a button click: deactivates the previous button,
     *        activates the new one, and emits modeChanged().
     */
    void onButtonClicked(MeasMode mode);

    MeasMode m_active{MeasMode::VDC};
    QMap<MeasMode, QPushButton*> m_buttons; ///< Mode → button mapping
};
