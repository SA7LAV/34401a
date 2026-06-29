from PyQt5.QtWidgets import (
    QDialog, QVBoxLayout, QTabWidget, QWidget,
    QFormLayout, QComboBox, QCheckBox, QDialogButtonBox, QLabel
)
from translations import tr, set_language, current_language
from config import app_config
from instrument import Instrument
from ui import DIALOG_STYLE


class SetupDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setModal(True)
        self.setMinimumWidth(550)
        self.setStyleSheet(DIALOG_STYLE)
        self._original_lang = current_language()
        self._build_ui()
        self._load_from_config()
        self.setWindowTitle(tr("setup_title"))

    def _build_ui(self):
        layout = QVBoxLayout(self)

        self._tabs = QTabWidget()
        self._tabs.addTab(self._build_language_tab(),    tr("tab_language"))
        self._tabs.addTab(self._build_interface_tab(),   tr("tab_interface"))
        self._tabs.addTab(self._build_measurement_tab(), tr("tab_measurement"))
        self._tabs.addTab(self._build_display_tab(),     tr("tab_display"))
        layout.addWidget(self._tabs)

        buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttons.accepted.connect(self._save_and_accept)
        buttons.rejected.connect(self.reject)
        layout.addWidget(buttons)

    def _build_language_tab(self) -> QWidget:
        w = QWidget()
        form = QFormLayout(w)
        self._lang_combo = QComboBox()
        self._lang_combo.addItem(tr("lang_de"), "de")
        self._lang_combo.addItem(tr("lang_en"), "en")
        self._lang_combo.currentIndexChanged.connect(self._on_lang_changed)
        form.addRow(QLabel(tr("lbl_language")), self._lang_combo)
        return w

    def _build_interface_tab(self) -> QWidget:
        w = QWidget()
        form = QFormLayout(w)

        self._port_combo = QComboBox()
        self._port_combo.addItems(Instrument.list_ports())
        form.addRow(QLabel(tr("lbl_port")), self._port_combo)

        self._baud_combo = QComboBox()
        self._baud_combo.addItems(["300", "600", "1200", "2400", "4800",
                                    "9600", "19200", "38400", "57600", "115200"])
        form.addRow(QLabel(tr("lbl_baudrate")), self._baud_combo)

        self._parity_combo = QComboBox()
        self._parity_combo.addItem(tr("parity_none"), "N")
        self._parity_combo.addItem(tr("parity_even"), "E")
        self._parity_combo.addItem(tr("parity_odd"),  "O")
        form.addRow(QLabel(tr("lbl_parity")), self._parity_combo)

        self._stopbits_combo = QComboBox()
        self._stopbits_combo.addItems(["1", "1.5", "2"])
        form.addRow(QLabel(tr("lbl_stopbits")), self._stopbits_combo)

        self._bytesize_combo = QComboBox()
        self._bytesize_combo.addItems(["7", "8"])
        form.addRow(QLabel(tr("lbl_bytesize")), self._bytesize_combo)

        self._timeout_combo = QComboBox()
        self._timeout_combo.addItems(["1.0", "2.0", "5.0", "9.0", "15.0"])
        form.addRow(QLabel(tr("lbl_timeout")), self._timeout_combo)

        return w

    def _build_measurement_tab(self) -> QWidget:
        w = QWidget()
        form = QFormLayout(w)

        self._sampling_combo = QComboBox()
        self._sampling_combo.addItem(tr("sampling_slow"),   "500")
        self._sampling_combo.addItem(tr("sampling_medium"), "200")
        self._sampling_combo.addItem(tr("sampling_fast"),   "50")
        form.addRow(QLabel(tr("lbl_sampling")), self._sampling_combo)

        self._nplc_combo = QComboBox()
        self._nplc_combo.addItems(["0.02", "0.2", "1", "10", "100"])
        form.addRow(QLabel(tr("lbl_nplc")), self._nplc_combo)

        self._autozero_chk = QCheckBox(tr("chk_autozero"))
        form.addRow(QLabel(tr("lbl_autozero")), self._autozero_chk)

        return w

    def _build_display_tab(self) -> QWidget:
        w = QWidget()
        form = QFormLayout(w)

        self._decimals_combo = QComboBox()
        self._decimals_combo.addItems(["2", "4", "6"])
        form.addRow(QLabel(tr("lbl_decimals")), self._decimals_combo)

        self._show_stats_chk = QCheckBox(tr("chk_show_stats"))
        form.addRow(QLabel(tr("lbl_show_stats")), self._show_stats_chk)

        self._overlay_chk = QCheckBox(tr("chk_overlay"))
        form.addRow(QLabel(tr("lbl_overlay")), self._overlay_chk)

        return w

    def _on_lang_changed(self) -> None:
        set_language(self._lang_combo.currentData())

    def reject(self):
        set_language(self._original_lang)
        super().reject()

    def _load_from_config(self):
        # Signale blockieren damit setCurrentIndex() kein set_language() auslöst
        self._lang_combo.blockSignals(True)
        lang_idx = self._lang_combo.findData(app_config.language)
        self._lang_combo.setCurrentIndex(max(0, lang_idx))
        self._lang_combo.blockSignals(False)

        ports = [self._port_combo.itemText(i) for i in range(self._port_combo.count())]
        if app_config.serial_port in ports:
            self._port_combo.setCurrentText(app_config.serial_port)
        self._baud_combo.setCurrentText(app_config.serial_baudrate)
        parity_idx = self._parity_combo.findData(app_config.serial_parity)
        self._parity_combo.setCurrentIndex(max(0, parity_idx))
        self._stopbits_combo.setCurrentText(app_config.serial_stopbits)
        self._bytesize_combo.setCurrentText(app_config.serial_bytesize)
        self._timeout_combo.setCurrentText(app_config.serial_timeout)

        sampling_idx = self._sampling_combo.findData(str(app_config.sampling_ms))
        self._sampling_combo.setCurrentIndex(max(0, sampling_idx))
        self._nplc_combo.setCurrentText(app_config.nplc)
        self._autozero_chk.setChecked(app_config.autozero)

        self._decimals_combo.setCurrentText(str(app_config.decimals))
        self._show_stats_chk.setChecked(app_config.show_stats)
        self._overlay_chk.setChecked(app_config.overlay_enabled)

    def _save_and_accept(self):
        app_config.set("serial", "port",     self._port_combo.currentText())
        app_config.set("serial", "baudrate", self._baud_combo.currentText())
        app_config.set("serial", "parity",   self._parity_combo.currentData())
        app_config.set("serial", "stopbits", self._stopbits_combo.currentText())
        app_config.set("serial", "bytesize", self._bytesize_combo.currentText())
        app_config.set("serial", "timeout",  self._timeout_combo.currentText())

        app_config.set("measurement", "sampling_ms", self._sampling_combo.currentData())
        app_config.set("measurement", "nplc",        self._nplc_combo.currentText())
        app_config.set("measurement", "autozero",    str(self._autozero_chk.isChecked()))

        app_config.set("display", "decimals",        self._decimals_combo.currentText())
        app_config.set("display", "show_stats",      str(self._show_stats_chk.isChecked()))
        app_config.set("display", "overlay_enabled", str(self._overlay_chk.isChecked()))

        app_config.set("app", "language", self._lang_combo.currentData())
        app_config.save()
        self.accept()
