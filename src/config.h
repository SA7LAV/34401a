#pragma once
#include <QSettings>
#include <QString>
#include <QPoint>

class AppConfig {
public:
    static AppConfig& instance();

    void save();
    QString get(const QString& section, const QString& key) const;
    void set(const QString& section, const QString& key, const QString& value);

    // App
    QString language() const;
    void setLanguage(const QString& v);

    // Measurement
    int samplingMs() const;
    QString nplc() const;
    bool autozero() const;

    // Display
    int decimals() const;
    bool showStats() const;
    bool overlayEnabled() const;

    // Overlay
    QString overlayColor() const;
    QString overlayFont() const;
    int overlaySize() const;
    QPoint overlayPos() const;
    void saveOverlayPos(int x, int y);

    // Serial
    QString serialPort() const;
    QString serialBaudrate() const;
    QString serialParity() const;
    QString serialStopbits() const;
    QString serialBytesize() const;
    QString serialTimeout() const;

private:
    AppConfig();
    QSettings m_settings;
};
