from PyQt5.QtCore import QObject, pyqtSignal

STRINGS = {
    "de": {
        # Hauptfenster
        "window_title":           "HP 34401A – Multimeter Control",
        "label_function":         "FUNKTION",
        "label_range":            "BEREICH",
        "btn_connect":            "Verbinden",
        "btn_disconnect":         "Trennen",
        "btn_stop":               "Stop",
        "btn_local":              "Local",
        "btn_setup":              "⚙ Setup",
        "status_not_connected":   "Nicht verbunden",
        "status_disconnected":    "Getrennt",
        "status_stopped":         "Messung gestoppt",
        "status_error":           "Fehler: {msg}",
        "status_connected":       "Verbunden: {port} @ {baudrate}",
        "dlg_conn_error_title":   "Verbindungsfehler",
        # ConnectionDialog
        "dlg_conn_title":         "Serielle Verbindung",
        "lbl_port":               "Port:",
        "lbl_baudrate":           "Baudrate:",
        "lbl_parity":             "Parität:",
        "lbl_stopbits":           "Stoppbits:",
        "lbl_bytesize":           "Datenbits:",
        "lbl_timeout":            "Timeout (s):",
        "parity_none":            "None (N)",
        "parity_even":            "Even (E)",
        "parity_odd":             "Odd (O)",
        # SetupDialog
        "setup_title":            "Einstellungen",
        "tab_language":           "Sprache",
        "tab_interface":          "Schnittstelle",
        "tab_measurement":        "Messung",
        "tab_display":            "Anzeige",
        "lbl_language":           "Sprache:",
        "lang_de":                "Deutsch",
        "lang_en":                "English",
        "lbl_sampling":           "Sampling-Rate:",
        "sampling_slow":          "Langsam (500 ms)",
        "sampling_medium":        "Mittel (200 ms)",
        "sampling_fast":          "Schnell (50 ms)",
        "lbl_nplc":               "NPLC:",
        "lbl_autozero":           "Auto-Zero:",
        "chk_autozero":           "Aktiviert",
        "lbl_decimals":           "Dezimalstellen:",
        "lbl_show_stats":         "Statistik anzeigen:",
        "chk_show_stats":         "MIN / MAX / AVG sichtbar",
    },
    "en": {
        "window_title":           "HP 34401A – Multimeter Control",
        "label_function":         "FUNCTION",
        "label_range":            "RANGE",
        "btn_connect":            "Connect",
        "btn_disconnect":         "Disconnect",
        "btn_stop":               "Stop",
        "btn_local":              "Local",
        "btn_setup":              "⚙ Setup",
        "status_not_connected":   "Not connected",
        "status_disconnected":    "Disconnected",
        "status_stopped":         "Measurement stopped",
        "status_error":           "Error: {msg}",
        "status_connected":       "Connected: {port} @ {baudrate}",
        "dlg_conn_error_title":   "Connection Error",
        "dlg_conn_title":         "Serial Connection",
        "lbl_port":               "Port:",
        "lbl_baudrate":           "Baud Rate:",
        "lbl_parity":             "Parity:",
        "lbl_stopbits":           "Stop Bits:",
        "lbl_bytesize":           "Data Bits:",
        "lbl_timeout":            "Timeout (s):",
        "parity_none":            "None (N)",
        "parity_even":            "Even (E)",
        "parity_odd":             "Odd (O)",
        "setup_title":            "Settings",
        "tab_language":           "Language",
        "tab_interface":          "Interface",
        "tab_measurement":        "Measurement",
        "tab_display":            "Display",
        "lbl_language":           "Language:",
        "lang_de":                "Deutsch",
        "lang_en":                "English",
        "lbl_sampling":           "Sampling Rate:",
        "sampling_slow":          "Slow (500 ms)",
        "sampling_medium":        "Medium (200 ms)",
        "sampling_fast":          "Fast (50 ms)",
        "lbl_nplc":               "NPLC:",
        "lbl_autozero":           "Auto-Zero:",
        "chk_autozero":           "Enabled",
        "lbl_decimals":           "Decimal Places:",
        "lbl_show_stats":         "Show Statistics:",
        "chk_show_stats":         "MIN / MAX / AVG visible",
    },
}

_current_lang = "de"


class _Notifier(QObject):
    language_changed = pyqtSignal(str)


_notifier = _Notifier()
language_changed = _notifier.language_changed


def tr(key: str, **kwargs) -> str:
    text = STRINGS.get(_current_lang, STRINGS["de"]).get(key, f"[{key}]")
    return text.format(**kwargs) if kwargs else text


def set_language(lang: str) -> None:
    global _current_lang
    if lang in STRINGS:
        _current_lang = lang
        _notifier.language_changed.emit(lang)


def current_language() -> str:
    return _current_lang
