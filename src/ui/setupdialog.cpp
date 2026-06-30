#include "setupdialog.h"
#include "../translations.h"
#include "../config.h"
#include "../instrument.h"
#include "dialogstyle.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QColorDialog>
#include <QColor>

SetupDialog::SetupDialog(QWidget* parent) : QDialog(parent)
{
    setModal(true);
    setMinimumWidth(550);
    setStyleSheet(dialogStyle());
    m_originalLang = Translations::instance().currentLanguage();
    buildUi();
    loadFromConfig();
    setWindowTitle(tl("setup_title"));
}

void SetupDialog::buildUi()
{
    auto* layout = new QVBoxLayout(this);
    auto* tabs   = new QTabWidget;
    tabs->addTab(buildLanguageTab(),    tl("tab_language"));
    tabs->addTab(buildInterfaceTab(),   tl("tab_interface"));
    tabs->addTab(buildMeasurementTab(), tl("tab_measurement"));
    tabs->addTab(buildDisplayTab(),     tl("tab_display"));
    tabs->addTab(buildOverlayTab(),     tl("tab_overlay"));
    layout->addWidget(tabs);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &SetupDialog::saveAndAccept);
    connect(buttons, &QDialogButtonBox::rejected, this, [this]() {
        Translations::instance().setLanguage(m_originalLang);
        reject();
    });
    layout->addWidget(buttons);
}

QWidget* SetupDialog::buildLanguageTab()
{
    auto* w    = new QWidget;
    auto* form = new QFormLayout(w);
    m_langCombo = new QComboBox;
    m_langCombo->addItem(tl("lang_de"), "de");
    m_langCombo->addItem(tl("lang_en"), "en");
    connect(m_langCombo, &QComboBox::currentIndexChanged,
            this, &SetupDialog::onLangChanged);
    form->addRow(new QLabel(tl("lbl_language")), m_langCombo);
    return w;
}

QWidget* SetupDialog::buildInterfaceTab()
{
    auto* w    = new QWidget;
    auto* form = new QFormLayout(w);

    m_portCombo = new QComboBox;
    m_portCombo->addItems(Instrument::listPorts());
    form->addRow(new QLabel(tl("lbl_port")), m_portCombo);

    m_baudCombo = new QComboBox;
    m_baudCombo->addItems({"300","600","1200","2400","4800","9600","19200","38400","57600","115200"});
    form->addRow(new QLabel(tl("lbl_baudrate")), m_baudCombo);

    m_parityCombo = new QComboBox;
    m_parityCombo->addItem(tl("parity_none"), "N");
    m_parityCombo->addItem(tl("parity_even"), "E");
    m_parityCombo->addItem(tl("parity_odd"),  "O");
    form->addRow(new QLabel(tl("lbl_parity")), m_parityCombo);

    m_stopbitsCombo = new QComboBox;
    m_stopbitsCombo->addItems({"1", "1.5", "2"});
    form->addRow(new QLabel(tl("lbl_stopbits")), m_stopbitsCombo);

    m_bytesizeCombo = new QComboBox;
    m_bytesizeCombo->addItems({"7", "8"});
    form->addRow(new QLabel(tl("lbl_bytesize")), m_bytesizeCombo);

    m_timeoutCombo = new QComboBox;
    m_timeoutCombo->addItems({"1.0", "2.0", "5.0", "9.0", "15.0"});
    form->addRow(new QLabel(tl("lbl_timeout")), m_timeoutCombo);

    return w;
}

QWidget* SetupDialog::buildMeasurementTab()
{
    auto* w    = new QWidget;
    auto* form = new QFormLayout(w);

    m_samplingCombo = new QComboBox;
    m_samplingCombo->addItem(tl("sampling_slow"),   "500");
    m_samplingCombo->addItem(tl("sampling_medium"), "200");
    m_samplingCombo->addItem(tl("sampling_fast"),   "50");
    form->addRow(new QLabel(tl("lbl_sampling")), m_samplingCombo);

    m_nplcCombo = new QComboBox;
    m_nplcCombo->addItems({"0.02", "0.2", "1", "10", "100"});
    form->addRow(new QLabel(tl("lbl_nplc")), m_nplcCombo);

    m_autozeroChk = new QCheckBox(tl("chk_autozero"));
    form->addRow(new QLabel(tl("lbl_autozero")), m_autozeroChk);

    return w;
}

QWidget* SetupDialog::buildDisplayTab()
{
    auto* w    = new QWidget;
    auto* form = new QFormLayout(w);

    m_decimalsCombo = new QComboBox;
    m_decimalsCombo->addItems({"2", "4", "6"});
    form->addRow(new QLabel(tl("lbl_decimals")), m_decimalsCombo);

    m_showStatsChk = new QCheckBox(tl("chk_show_stats"));
    form->addRow(new QLabel(tl("lbl_show_stats")), m_showStatsChk);

    return w;
}

QWidget* SetupDialog::buildOverlayTab()
{
    auto* w    = new QWidget;
    auto* form = new QFormLayout(w);

    m_overlayChk = new QCheckBox(tl("chk_overlay"));
    form->addRow(new QLabel(tl("lbl_overlay")), m_overlayChk);

    m_overlayColor = AppConfig::instance().overlayColor();
    m_colorBtn = new QPushButton;
    m_colorBtn->setFixedSize(80, 28);
    m_colorBtn->setStyleSheet(
        QString("background: %1; border: 1px solid #2A3560;").arg(m_overlayColor));
    connect(m_colorBtn, &QPushButton::clicked, this, &SetupDialog::pickOverlayColor);
    form->addRow(new QLabel(tl("lbl_overlay_color")), m_colorBtn);

    m_overlayFontCombo = new QComboBox;
    m_overlayFontCombo->addItems({
        "Courier New", "Liberation Mono", "DejaVu Sans Mono",
        "Consolas", "Arial", "Ubuntu",
    });
    form->addRow(new QLabel(tl("lbl_overlay_font")), m_overlayFontCombo);

    m_overlaySizeCombo = new QComboBox;
    m_overlaySizeCombo->addItems({"48","56","64","72","80","96","120"});
    form->addRow(new QLabel(tl("lbl_overlay_size")), m_overlaySizeCombo);

    return w;
}

void SetupDialog::pickOverlayColor()
{
    QColor c = QColorDialog::getColor(QColor(m_overlayColor), this);
    if (c.isValid()) {
        m_overlayColor = c.name();
        m_colorBtn->setStyleSheet(
            QString("background: %1; border: 1px solid #2A3560;").arg(m_overlayColor));
    }
}

void SetupDialog::onLangChanged()
{
    Translations::instance().setLanguage(m_langCombo->currentData().toString());
}

void SetupDialog::loadFromConfig()
{
    auto& cfg = AppConfig::instance();

    // Block signal to avoid triggering language change on load
    m_langCombo->blockSignals(true);
    int langIdx = m_langCombo->findData(cfg.language());
    m_langCombo->setCurrentIndex(langIdx >= 0 ? langIdx : 0);
    m_langCombo->blockSignals(false);

    QStringList ports = Instrument::listPorts();
    if (ports.contains(cfg.serialPort()))
        m_portCombo->setCurrentText(cfg.serialPort());
    m_baudCombo->setCurrentText(cfg.serialBaudrate());
    int parityIdx = m_parityCombo->findData(cfg.serialParity());
    m_parityCombo->setCurrentIndex(parityIdx >= 0 ? parityIdx : 0);
    m_stopbitsCombo->setCurrentText(cfg.serialStopbits());
    m_bytesizeCombo->setCurrentText(cfg.serialBytesize());
    m_timeoutCombo->setCurrentText(cfg.serialTimeout());

    int sampIdx = m_samplingCombo->findData(QString::number(cfg.samplingMs()));
    m_samplingCombo->setCurrentIndex(sampIdx >= 0 ? sampIdx : 0);
    m_nplcCombo->setCurrentText(cfg.nplc());
    m_autozeroChk->setChecked(cfg.autozero());

    m_decimalsCombo->setCurrentText(QString::number(cfg.decimals()));
    m_showStatsChk->setChecked(cfg.showStats());

    m_overlayChk->setChecked(cfg.overlayEnabled());
    m_overlayColor = cfg.overlayColor();
    m_colorBtn->setStyleSheet(
        QString("background: %1; border: 1px solid #2A3560;").arg(m_overlayColor));
    m_overlayFontCombo->setCurrentText(cfg.overlayFont());
    m_overlaySizeCombo->setCurrentText(QString::number(cfg.overlaySize()));
}

void SetupDialog::saveAndAccept()
{
    auto& cfg = AppConfig::instance();
    cfg.set("serial", "port",     m_portCombo->currentText());
    cfg.set("serial", "baudrate", m_baudCombo->currentText());
    cfg.set("serial", "parity",   m_parityCombo->currentData().toString());
    cfg.set("serial", "stopbits", m_stopbitsCombo->currentText());
    cfg.set("serial", "bytesize", m_bytesizeCombo->currentText());
    cfg.set("serial", "timeout",  m_timeoutCombo->currentText());

    cfg.set("measurement", "sampling_ms", m_samplingCombo->currentData().toString());
    cfg.set("measurement", "nplc",        m_nplcCombo->currentText());
    cfg.set("measurement", "autozero",    m_autozeroChk->isChecked() ? "true" : "false");

    cfg.set("display", "decimals",        m_decimalsCombo->currentText());
    cfg.set("display", "show_stats",      m_showStatsChk->isChecked() ? "true" : "false");
    cfg.set("display", "overlay_enabled", m_overlayChk->isChecked() ? "true" : "false");

    cfg.set("overlay", "color", m_overlayColor);
    cfg.set("overlay", "font",  m_overlayFontCombo->currentText());
    cfg.set("overlay", "size",  m_overlaySizeCombo->currentText());
    cfg.set("app", "language",  m_langCombo->currentData().toString());

    cfg.save();
    accept();
}
