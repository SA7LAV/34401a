/**
 * @file config.h
 * @brief Persistent application configuration backed by a QSettings INI file.
 *
 * AppConfig is a Meyer's singleton that wraps @c QSettings and provides
 * typed accessors for every preference.  The backing file is:
 *   @c ~/.config/hp34401a/serial_config.ini
 *
 * The directory is created on first save() if it does not yet exist.
 *
 * @note AppConfig is not thread-safe.  Access it from the main thread only.
 */
#pragma once
#include <QSettings>
#include <QString>
#include <QPoint>

class AppConfig {
public:
    /** @brief Returns the single application-wide AppConfig instance. */
    static AppConfig& instance();

    /**
     * @brief Flushes all pending changes to disk via QSettings::sync().
     *
     * Also ensures the config directory exists before syncing.
     * saveOverlayPos() calls this implicitly; all other setters only buffer
     * the value in memory until save() is called explicitly (e.g. in
     * SetupDialog::saveAndAccept()).
     */
    void save();

    /**
     * @brief Low-level key read.  Prefer the typed accessors below.
     * @param section  INI section name (e.g. "serial").
     * @param key      Key within the section (e.g. "baudrate").
     * @return Stored value as a string, or an empty string if absent.
     */
    QString get(const QString& section, const QString& key) const;

    /**
     * @brief Low-level key write.  Prefer the typed accessors below.
     * @param section  INI section name.
     * @param key      Key within the section.
     * @param value    New value; stored as a string.
     */
    void set(const QString& section, const QString& key, const QString& value);

    // -------------------------------------------------------------------------
    /** @name Application */
    ///@{
    /** @brief Returns the active UI language code ("de", "en", or "sv"). */
    QString language() const;
    void setLanguage(const QString& v);
    ///@}

    // -------------------------------------------------------------------------
    /** @name Measurement */
    ///@{
    /** @brief Polling interval in milliseconds (50, 200, or 500). */
    int samplingMs() const;

    /**
     * @brief Number of power-line cycles used for each A/D integration.
     *
     * Returned as a string because QComboBox data is string-based and the
     * SCPI command is built by direct string substitution.
     * Valid values: "0.02", "0.2", "1", "10", "100".
     */
    QString nplc() const;

    /** @brief Returns @c true when the instrument's auto-zero feature is enabled. */
    bool autozero() const;
    ///@}

    // -------------------------------------------------------------------------
    /** @name Display */
    ///@{
    /** @brief Number of decimal places shown in the main readout (2, 4, or 6). */
    int decimals() const;

    /** @brief Returns @c true when the MIN/MAX/AVG statistics strip is visible. */
    bool showStats() const;

    /** @brief Returns @c true when the OBS streaming overlay window is active. */
    bool overlayEnabled() const;
    ///@}

    // -------------------------------------------------------------------------
    /** @name Overlay */
    ///@{
    /** @brief Overlay text colour as a CSS hex string (e.g. "#00C0FF"). */
    QString overlayColor() const;

    /**
     * @brief Overlay background colour as a CSS hex string (e.g. "#00FF00").
     *
     * Defaults to pure green for OBS chroma keying, but is user-configurable
     * so the overlay can be blended over any background.
     */
    QString overlayBgColor() const;

    /** @brief Font family name for the overlay readout. */
    QString overlayFont() const;

    /** @brief Overlay font point size. */
    int overlaySize() const;

    /**
     * @brief Last saved screen position of the overlay window.
     * @return A QPoint with x and y set to -1 if no position has been saved yet.
     */
    QPoint overlayPos() const;

    /**
     * @brief Persists the overlay window position and immediately calls save().
     *
     * Unlike other setters, this one writes to disk immediately so that the
     * position is not lost if the app is closed without visiting the Setup dialog.
     */
    void saveOverlayPos(int x, int y);
    ///@}

    // -------------------------------------------------------------------------
    /** @name Serial port */
    ///@{
    QString serialPort()     const; ///< Device path, e.g. "/dev/ttyUSB0"
    QString serialBaudrate() const; ///< Baud rate as a string, e.g. "9600"
    QString serialParity()   const; ///< Single-char code: "N", "E", or "O"
    QString serialStopbits() const; ///< "1", "1.5", or "2"
    QString serialBytesize() const; ///< "7" or "8"
    QString serialTimeout()  const; ///< Read timeout in seconds, e.g. "9.0"
    ///@}

private:
    AppConfig();
    QSettings m_settings;
};
