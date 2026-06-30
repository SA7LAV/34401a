#include "translations.h"

using LangMap = QMap<QString, QString>;
using StringTable = QMap<QString, LangMap>;

static const StringTable STRINGS = {
    {"de", {
        {"window_title",         "HP 34401A – Multimeter Control"},
        {"label_function",       "FUNKTION"},
        {"label_range",          "BEREICH"},
        {"btn_connect",          "Verbinden"},
        {"btn_disconnect",       "Trennen"},
        {"btn_stop",             "Stop"},
        {"btn_local",            "Local"},
        {"btn_setup",            "⚙ Setup"},
        {"status_not_connected", "Nicht verbunden"},
        {"status_connecting",    "Verbinde..."},
        {"status_disconnected",  "Getrennt"},
        {"status_stopped",       "Messung gestoppt"},
        {"status_error",         "Fehler: {msg}"},
        {"status_connected",     "Verbunden: {port} @ {baudrate}"},
        {"dlg_conn_error_title", "Verbindungsfehler"},
        {"dlg_conn_title",       "Serielle Verbindung"},
        {"lbl_port",             "Port:"},
        {"lbl_baudrate",         "Baudrate:"},
        {"lbl_parity",           "Parität:"},
        {"lbl_stopbits",         "Stoppbits:"},
        {"lbl_bytesize",         "Datenbits:"},
        {"lbl_timeout",          "Timeout (s):"},
        {"parity_none",          "None (N)"},
        {"parity_even",          "Even (E)"},
        {"parity_odd",           "Odd (O)"},
        {"setup_title",          "Einstellungen"},
        {"tab_language",         "Sprache"},
        {"tab_interface",        "Schnittstelle"},
        {"tab_measurement",      "Messung"},
        {"tab_display",          "Anzeige"},
        {"tab_overlay",          "Overlay"},
        {"lbl_language",         "Sprache:"},
        {"lang_de",              "Deutsch"},
        {"lang_en",              "English"},
        {"lbl_sampling",         "Sampling-Rate:"},
        {"sampling_slow",        "Langsam (500 ms)"},
        {"sampling_medium",      "Mittel (200 ms)"},
        {"sampling_fast",        "Schnell (50 ms)"},
        {"lbl_nplc",             "NPLC:"},
        {"lbl_autozero",         "Auto-Zero:"},
        {"chk_autozero",         "Aktiviert"},
        {"lbl_decimals",         "Dezimalstellen:"},
        {"lbl_show_stats",       "Statistik anzeigen:"},
        {"chk_show_stats",       "MIN / MAX / AVG sichtbar"},
        {"lbl_overlay",          "OBS Overlay:"},
        {"chk_overlay",          "Overlay-Fenster für Streaming anzeigen"},
        {"lbl_overlay_color",    "Schriftfarbe:"},
        {"lbl_overlay_font",     "Schriftart:"},
        {"lbl_overlay_size",     "Schriftgröße:"},
    }},
    {"en", {
        {"window_title",         "HP 34401A – Multimeter Control"},
        {"label_function",       "FUNCTION"},
        {"label_range",          "RANGE"},
        {"btn_connect",          "Connect"},
        {"btn_disconnect",       "Disconnect"},
        {"btn_stop",             "Stop"},
        {"btn_local",            "Local"},
        {"btn_setup",            "⚙ Setup"},
        {"status_not_connected", "Not connected"},
        {"status_connecting",    "Connecting..."},
        {"status_disconnected",  "Disconnected"},
        {"status_stopped",       "Measurement stopped"},
        {"status_error",         "Error: {msg}"},
        {"status_connected",     "Connected: {port} @ {baudrate}"},
        {"dlg_conn_error_title", "Connection Error"},
        {"dlg_conn_title",       "Serial Connection"},
        {"lbl_port",             "Port:"},
        {"lbl_baudrate",         "Baud Rate:"},
        {"lbl_parity",           "Parity:"},
        {"lbl_stopbits",         "Stop Bits:"},
        {"lbl_bytesize",         "Data Bits:"},
        {"lbl_timeout",          "Timeout (s):"},
        {"parity_none",          "None (N)"},
        {"parity_even",          "Even (E)"},
        {"parity_odd",           "Odd (O)"},
        {"setup_title",          "Settings"},
        {"tab_language",         "Language"},
        {"tab_interface",        "Interface"},
        {"tab_measurement",      "Measurement"},
        {"tab_display",          "Display"},
        {"tab_overlay",          "Overlay"},
        {"lbl_language",         "Language:"},
        {"lang_de",              "Deutsch"},
        {"lang_en",              "English"},
        {"lbl_sampling",         "Sampling Rate:"},
        {"sampling_slow",        "Slow (500 ms)"},
        {"sampling_medium",      "Medium (200 ms)"},
        {"sampling_fast",        "Fast (50 ms)"},
        {"lbl_nplc",             "NPLC:"},
        {"lbl_autozero",         "Auto-Zero:"},
        {"chk_autozero",         "Enabled"},
        {"lbl_decimals",         "Decimal Places:"},
        {"lbl_show_stats",       "Show Statistics:"},
        {"chk_show_stats",       "MIN / MAX / AVG visible"},
        {"lbl_overlay",          "OBS Overlay:"},
        {"chk_overlay",          "Show overlay window for streaming"},
        {"lbl_overlay_color",    "Font Color:"},
        {"lbl_overlay_font",     "Font:"},
        {"lbl_overlay_size",     "Font Size:"},
    }},
};

Translations& Translations::instance()
{
    static Translations inst;
    return inst;
}

Translations::Translations(QObject* parent) : QObject(parent) {}

QString Translations::tr(const QString& key) const
{
    auto langIt = STRINGS.find(m_lang);
    if (langIt == STRINGS.end()) langIt = STRINGS.find("de");
    if (langIt == STRINGS.end()) return "[" + key + "]";
    auto it = langIt->find(key);
    return it != langIt->end() ? *it : "[" + key + "]";
}

QString Translations::tr(const QString& key, const QMap<QString, QString>& args) const
{
    QString text = tr(key);
    for (auto it = args.begin(); it != args.end(); ++it)
        text.replace("{" + it.key() + "}", it.value());
    return text;
}

void Translations::setLanguage(const QString& lang)
{
    if (!STRINGS.contains(lang)) return;
    m_lang = lang;
    emit languageChanged(lang);
}
