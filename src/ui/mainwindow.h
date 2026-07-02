/**
 * @file mainwindow.h
 * @brief Top-level application window for the 34401A Multimeter GUI.
 *
 * MainWindow is the central coordinator: it owns the Instrument connection
 * object and wires it to the display, mode selector, range selector, and
 * optional OBS overlay window.  The three-column layout consists of:
 *  - Left:   ModePanel (function buttons)
 *  - Centre: DisplayWidget (large readout + MIN/MAX/AVG) + RangePanel
 *  - Right:  Action buttons (Setup, Connect, Stop, Local)
 */
#pragma once
#include <QMainWindow>
#include "../models.h"

class Instrument;
class DisplayWidget;
class ModePanel;
class RangePanel;
class OverlayWindow;
class QLabel;
class QPushButton;
class QStatusBar;

/**
 * @brief Main application window.
 *
 * Responsibilities:
 *  - Manages the Instrument lifecycle (connect, disconnect, error handling).
 *  - Forwards mode/range changes to the instrument as SCPI commands.
 *  - Creates and destroys the OverlayWindow on demand based on configuration.
 *  - Re-translates the entire UI when the language is changed live.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    /**
     * @brief Tears down the overlay, stops sampling, and disconnects before
     *        allowing the window to close.
     */
    void closeEvent(QCloseEvent* event) override;

private:
    /** @brief Constructs all child widgets and layouts. */
    void buildUi();

    /** @brief Connects signals and slots between child objects. */
    void wireSignals();

    /**
     * @brief Creates or destroys the overlay window according to @p enabled.
     *
     * Uses lazy construction: the overlay is only allocated the first time it
     * is enabled, and the measurement signal is connected/disconnected
     * dynamically to avoid delivering values to a destroyed window.
     */
    void applyOverlay(bool enabled);

    /**
     * @brief Sends the current NPLC setting to the instrument for @p mode.
     *
     * NPLC is not applicable to FREQ, PERIOD, DIODE, or CONT; those modes
     * are silently skipped.  Must be called after mode changes and after
     * a new connection is established.
     */
    void sendNplc(MeasMode mode);

    /** @brief Re-applies all translatable strings after a language change. */
    void retranslateUi();

    // -- Slot handlers --
    void onSetupClicked();
    void onConnectClicked();
    void onStopClicked();
    /** @brief Sends SYST:LOC to return the instrument to local (front-panel) control. */
    void onLocalClicked();
    /** @brief Switches mode, updates the range panel, resets stats, sends SCPI. */
    void onModeChanged(MeasMode mode);
    /** @brief Sends the selected range command, or AUTO if @p scpiCmd is empty. */
    void onRangeSelected(const QString& scpiCmd);
    /**
     * @brief Shows a mode-appropriate overload indicator on the display/overlay.
     *
     * Triggered by Instrument::overloadDetected(). The text follows the
     * front-panel convention: "OVL.D" for resistance, "OPEN" for diode and
     * continuity, and "OVLD" for all other functions.
     */
    void onOverload();
    void onConnected();
    void onConnectionFailed(const QString& msg);
    void onError(const QString& msg);

    // -- Child widgets --
    Instrument*    m_instrument{nullptr};
    DisplayWidget* m_display{nullptr};
    ModePanel*     m_modePanel{nullptr};
    RangePanel*    m_rangePanel{nullptr};
    OverlayWindow* m_overlay{nullptr};  ///< nullptr when overlay is disabled
    QLabel*        m_lblFunction{nullptr};
    QLabel*        m_lblRange{nullptr};
    QPushButton*   m_setupBtn{nullptr};
    QPushButton*   m_connectBtn{nullptr};
    QPushButton*   m_stopBtn{nullptr};
    QPushButton*   m_localBtn{nullptr};
    QStatusBar*    m_status{nullptr};

    MeasMode m_currentMode{MeasMode::VDC};
    QString  m_connectPort; ///< Cached for the "Connected: port @ baud" status message
    QString  m_connectBaud; ///< Cached for the "Connected: port @ baud" status message
};
