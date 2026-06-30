/**
 * @file overlaywindow.h
 * @brief Frameless always-on-top overlay for OBS screen capture.
 *
 * The window has a solid #00FF00 (pure green) background so it can be keyed
 * out in OBS (or any other compositor that supports chroma keying), leaving
 * only the measurement text visible over any background in a stream or recording.
 *
 * The overlay is draggable and saves its screen position to AppConfig so it
 * is restored to the same location on the next launch.  Position writes are
 * debounced through a 500 ms single-shot timer to avoid hammering QSettings
 * during a drag operation.
 *
 * Font family, text colour, and font size are all configurable via the Setup
 * dialog (Overlay tab) and applied by calling applyStyle().
 */
#pragma once
#include <QWidget>
#include <QPoint>

class QLabel;
class QTimer;

/**
 * @brief Floating measurement overlay intended for use with OBS Studio.
 *
 * Created and destroyed dynamically by MainWindow based on the
 * "overlay_enabled" configuration flag.  The measurement signal is connected
 * when the overlay is created and disconnected when it is destroyed.
 */
class OverlayWindow : public QWidget {
    Q_OBJECT
public:
    explicit OverlayWindow(QWidget* parent = nullptr);

    /** @brief Sets the unit string (e.g. "VDC") shown after the value. */
    void setUnit(const QString& unit);

    /**
     * @brief Re-reads font, colour, and size from AppConfig and applies them.
     *
     * Called by MainWindow after the Setup dialog is accepted.
     */
    void applyStyle();

public slots:
    /**
     * @brief Updates the displayed value.
     *
     * @param rawValue  Raw instrument reading; scaled and prefixed by formatValue().
     *                  The SI prefix label is shown or hidden dynamically so
     *                  the unit sits flush against the number.
     */
    void updateValue(double rawValue);

protected:
    /** @brief Begins a native system move (Wayland/X11) or falls back to manual drag. */
    void mousePressEvent(QMouseEvent* event) override;
    /** @brief Continues manual drag when native system move is unavailable. */
    void mouseMoveEvent(QMouseEvent* event) override;
    /** @brief Ends manual drag and resets the drag anchor. */
    void mouseReleaseEvent(QMouseEvent* event) override;
    /** @brief Starts the debounce timer that eventually calls savePos(). */
    void moveEvent(QMoveEvent* event) override;

private:
    /**
     * @brief Moves the window to the last saved position, or to the top-right
     *        corner of the primary screen if no position has been saved yet.
     */
    void restorePos();

    /** @brief Writes the current window position to AppConfig. */
    void savePos();

    QLabel*  m_valLabel{nullptr};    ///< Numeric part of the reading
    QLabel*  m_prefixLabel{nullptr}; ///< SI prefix (hidden when empty)
    QLabel*  m_unitLabel{nullptr};   ///< Unit string
    QTimer*  m_saveTimer{nullptr};   ///< 500 ms debounce timer for position saving
    QPoint   m_dragPos;              ///< Manual-drag anchor; null when not dragging
    QString  m_unit{"VDC"};
};
