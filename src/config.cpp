#include "config.h"
#include <QDir>
#include <QStandardPaths>

static QString configPath()
{
    return QDir::homePath() + "/.config/hp34401a/serial_config.ini";
}

AppConfig& AppConfig::instance()
{
    static AppConfig inst;
    return inst;
}

AppConfig::AppConfig()
    : m_settings(configPath(), QSettings::IniFormat)
{
    // Set defaults only when key is absent
    auto def = [&](const QString& key, const QString& val) {
        if (!m_settings.contains(key)) m_settings.setValue(key, val);
    };
    def("serial/port",             "");
    def("serial/baudrate",         "9600");
    def("serial/parity",           "N");
    def("serial/stopbits",         "1");
    def("serial/bytesize",         "8");
    def("serial/timeout",          "9.0");
    def("measurement/sampling_ms", "50");
    def("measurement/nplc",        "1");
    def("measurement/counts",      "100000");
    def("measurement/autozero",    "true");
    def("display/decimals",        "4");
    def("display/show_stats",      "true");
    def("display/overlay_enabled", "false");
    def("overlay/color",           "#00C0FF");
    def("overlay/bg_color",        "#00FF00");
    def("overlay/font",            "Courier New");
    def("overlay/size",            "72");
    def("overlay/pos_x",           "-1");
    def("overlay/pos_y",           "-1");
    def("app/language",            "de");
}

void AppConfig::save()
{
    QDir().mkpath(QFileInfo(configPath()).dir().path());
    m_settings.sync();
}

QString AppConfig::get(const QString& section, const QString& key) const
{
    return m_settings.value(section + "/" + key).toString();
}

void AppConfig::set(const QString& section, const QString& key, const QString& value)
{
    m_settings.setValue(section + "/" + key, value);
}

QString AppConfig::language() const       { return get("app", "language"); }
void AppConfig::setLanguage(const QString& v) { set("app", "language", v); }

int AppConfig::samplingMs() const         { return get("measurement", "sampling_ms").toInt(); }
QString AppConfig::nplc() const           { return get("measurement", "nplc"); }
int AppConfig::counts() const             { return get("measurement", "counts").toInt(); }
bool AppConfig::autozero() const          { return get("measurement", "autozero").toLower() == "true"; }

int AppConfig::decimals() const           { return get("display", "decimals").toInt(); }
bool AppConfig::showStats() const         { return get("display", "show_stats").toLower() == "true"; }
bool AppConfig::overlayEnabled() const    { return get("display", "overlay_enabled").toLower() == "true"; }

QString AppConfig::overlayColor() const   { return get("overlay", "color"); }
QString AppConfig::overlayBgColor() const { return get("overlay", "bg_color"); }
QString AppConfig::overlayFont() const    { return get("overlay", "font"); }
int AppConfig::overlaySize() const        { return get("overlay", "size").toInt(); }

QPoint AppConfig::overlayPos() const
{
    return {get("overlay", "pos_x").toInt(), get("overlay", "pos_y").toInt()};
}

void AppConfig::saveOverlayPos(int x, int y)
{
    set("overlay", "pos_x", QString::number(x));
    set("overlay", "pos_y", QString::number(y));
    save();
}

QString AppConfig::serialPort() const     { return get("serial", "port"); }
QString AppConfig::serialBaudrate() const { return get("serial", "baudrate"); }
QString AppConfig::serialParity() const   { return get("serial", "parity"); }
QString AppConfig::serialStopbits() const { return get("serial", "stopbits"); }
QString AppConfig::serialBytesize() const { return get("serial", "bytesize"); }
QString AppConfig::serialTimeout() const  { return get("serial", "timeout"); }
