#include "mainwindow.h"
#include "displaywidget.h"
#include "modepanel.h"
#include "rangepanel.h"
#include "connectiondialog.h"
#include "overlaywindow.h"
#include "setupdialog.h"
#include "../instrument.h"
#include "../translations.h"
#include "../config.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QMessageBox>
#include <QCloseEvent>

static const char* BTN_SS =
    "QPushButton { background: #1A2040; color: #00C0FF; "
    "border: 1px solid #00C0FF; border-radius: 4px; padding: 6px 14px; }"
    "QPushButton:hover { background: #203060; }"
    "QPushButton:disabled { color: #444; border-color: #444; }";

static const char* STOP_SS =
    "QPushButton { background: #400A0A; color: #FF4444; "
    "border: 1px solid #FF4444; border-radius: 4px; padding: 6px 14px; }"
    "QPushButton:hover { background: #601010; }";

static const QMap<MeasMode, QString> AUTO_RANGE_CMD = {
    {MeasMode::VDC,    "VOLT:DC:RANG:AUTO ON"},
    {MeasMode::ADC,    "CURR:DC:RANG:AUTO ON"},
    {MeasMode::VAC,    "VOLT:AC:RANG:AUTO ON"},
    {MeasMode::AAC,    "CURR:AC:RANG:AUTO ON"},
    {MeasMode::OHM2,   "RES:RANG:AUTO ON"},
    {MeasMode::OHM4,   "FRES:RANG:AUTO ON"},
    {MeasMode::FREQ,   "FREQ:VOLT:RANG:AUTO ON"},
    {MeasMode::PERIOD, "PER:VOLT:RANG:AUTO ON"},
};

// Overload indicators mirroring the 34401A front-panel annunciators.  Modes not
// listed here fall back to the generic "OVLD".
static const QMap<MeasMode, QString> OVERLOAD_TEXT = {
    {MeasMode::OHM2,  "OVL.D"},
    {MeasMode::OHM4,  "OVL.D"},
    {MeasMode::DIODE, "OPEN"},
    {MeasMode::CONT,  "OPEN"},
};

static const QMap<MeasMode, QString> NPLC_CMD = {
    {MeasMode::VDC,    "SENS:VOLT:DC:NPLC %1"},
    {MeasMode::ADC,    "SENS:CURR:DC:NPLC %1"},
    {MeasMode::VAC,    "SENS:VOLT:AC:NPLC %1"},
    {MeasMode::AAC,    "SENS:CURR:AC:NPLC %1"},
    {MeasMode::OHM2,   "SENS:RES:NPLC %1"},
    {MeasMode::OHM4,   "SENS:FRES:NPLC %1"},
};

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setMinimumSize(820, 500);
    setStyleSheet("background-color: #0A0A1A;");
    m_instrument = new Instrument(this);
    buildUi();
    wireSignals();
    applyOverlay(AppConfig::instance().overlayEnabled());
}

void MainWindow::buildUi()
{
    auto* central = new QWidget;
    setCentralWidget(central);
    auto* root = new QHBoxLayout(central);

    // Left: mode panel
    auto* left = new QVBoxLayout;
    m_lblFunction = new QLabel(tl("label_function"));
    m_lblFunction->setStyleSheet("color: #6080A0; font-size: 10px;");
    left->addWidget(m_lblFunction);
    m_modePanel = new ModePanel;
    left->addWidget(m_modePanel);
    left->addStretch();
    root->addLayout(left);

    // Center: display + range panel
    auto* center = new QVBoxLayout;
    m_display = new DisplayWidget;
    center->addWidget(m_display, 3);
    m_lblRange = new QLabel(tl("label_range"));
    m_lblRange->setStyleSheet("color: #6080A0; font-size: 10px;");
    center->addWidget(m_lblRange);
    m_rangePanel = new RangePanel;
    center->addWidget(m_rangePanel);
    root->addLayout(center, 1);

    // Right: action buttons
    auto* right = new QVBoxLayout;
    m_setupBtn   = new QPushButton(tl("btn_setup"));   m_setupBtn->setStyleSheet(BTN_SS);
    m_connectBtn = new QPushButton(tl("btn_connect")); m_connectBtn->setStyleSheet(BTN_SS);
    m_stopBtn    = new QPushButton(tl("btn_stop"));    m_stopBtn->setStyleSheet(STOP_SS);
    m_localBtn   = new QPushButton(tl("btn_local"));   m_localBtn->setStyleSheet(BTN_SS);
    m_stopBtn->setEnabled(false);
    m_localBtn->setEnabled(false);
    right->addWidget(m_setupBtn);
    right->addWidget(m_connectBtn);
    right->addWidget(m_stopBtn);
    right->addWidget(m_localBtn);
    right->addStretch();
    root->addLayout(right);

    m_status = new QStatusBar;
    m_status->setStyleSheet("color: #6080A0;");
    m_status->showMessage(tl("status_not_connected"));
    auto* verLabel = new QLabel("v" APP_VERSION);
    verLabel->setStyleSheet("color: #405060; font-size: 10px; padding-right: 4px;");
    m_status->addPermanentWidget(verLabel);
    setStatusBar(m_status);
    setWindowTitle(tl("window_title"));
}

void MainWindow::wireSignals()
{
    connect(m_instrument, &Instrument::measurementReceived,
            m_display,   &DisplayWidget::updateValue);
    connect(m_instrument, &Instrument::overloadDetected,
            this, &MainWindow::onOverload);
    connect(m_instrument, &Instrument::errorOccurred,
            this, &MainWindow::onError);
    connect(m_instrument, &Instrument::connected,
            this, &MainWindow::onConnected);
    connect(m_instrument, &Instrument::connectionFailed,
            this, &MainWindow::onConnectionFailed);

    connect(m_modePanel,  &ModePanel::modeChanged,
            this, &MainWindow::onModeChanged);
    connect(m_rangePanel, &RangePanel::rangeSelected,
            this, &MainWindow::onRangeSelected);

    connect(m_setupBtn,   &QPushButton::clicked, this, &MainWindow::onSetupClicked);
    connect(m_connectBtn, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(m_stopBtn,    &QPushButton::clicked, this, &MainWindow::onStopResumeClicked);
    connect(m_localBtn,   &QPushButton::clicked, this, &MainWindow::onLocalClicked);

    connect(&Translations::instance(), &Translations::languageChanged,
            this, [this](const QString&) { retranslateUi(); });

    m_modePanel->select(MeasMode::VDC);
}

void MainWindow::retranslateUi()
{
    setWindowTitle(tl("window_title"));
    m_lblFunction->setText(tl("label_function"));
    m_lblRange->setText(tl("label_range"));
    m_setupBtn->setText(tl("btn_setup"));
    m_stopBtn->setText(m_sampling ? tl("btn_stop") : tl("btn_resume"));
    m_localBtn->setText(tl("btn_local"));
    if (m_instrument->isConnected()) {
        m_connectBtn->setText(tl("btn_disconnect"));
    } else {
        m_connectBtn->setText(tl("btn_connect"));
        m_status->showMessage(tl("status_not_connected"));
    }
    m_display->applyDisplayConfig();
}

void MainWindow::applyOverlay(bool enabled)
{
    if (enabled) {
        if (!m_overlay) {
            m_overlay = new OverlayWindow;
            connect(m_instrument, &Instrument::measurementReceived,
                    m_overlay, &OverlayWindow::updateValue);
        }
        m_overlay->setUnit(MODES[m_currentMode].unit);
        m_overlay->show();
    } else {
        if (m_overlay) {
            QObject::disconnect(m_instrument, &Instrument::measurementReceived,
                                m_overlay,    &OverlayWindow::updateValue);
            m_overlay->close();
            m_overlay->deleteLater();
            m_overlay = nullptr;
        }
    }
}

void MainWindow::sendNplc(MeasMode mode)
{
    auto it = NPLC_CMD.find(mode);
    if (it != NPLC_CMD.end())
        m_instrument->sendCommand(it->arg(AppConfig::instance().nplc()));
}

void MainWindow::onSetupClicked()
{
    SetupDialog dlg(this);
    dlg.exec();
    m_display->applyDisplayConfig();
    applyOverlay(AppConfig::instance().overlayEnabled());
    if (m_overlay) m_overlay->applyStyle();
}

void MainWindow::onConnectClicked()
{
    if (m_instrument->isConnected()) {
        m_instrument->disconnectDevice();
        m_connectBtn->setText(tl("btn_connect"));
        m_connectBtn->setEnabled(true);
        setSamplingState(true); // reset label to "Stop" for the next session
        m_stopBtn->setEnabled(false);
        m_localBtn->setEnabled(false);
        m_status->showMessage(tl("status_disconnected"));
        return;
    }

    ConnectionDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    m_connectPort = dlg.port();
    m_connectBaud = QString::number(dlg.baudrate());
    m_connectBtn->setEnabled(false);
    m_status->showMessage(tl("status_connecting"));

    QString az = AppConfig::instance().autozero() ? "ON" : "OFF";
    auto nplcIt = NPLC_CMD.find(m_currentMode);
    QString nplcCmd = (nplcIt != NPLC_CMD.end())
                      ? nplcIt->arg(AppConfig::instance().nplc())
                      : QString();

    QStringList setupCmds = {"SYST:REM",
                              QString("SENS:ZERO:AUTO %1").arg(az),
                              MODES[m_currentMode].confCmd};
    if (!nplcCmd.isEmpty()) setupCmds << nplcCmd;

    m_instrument->connectAsync(
        dlg.port(), dlg.baudrate(), dlg.parity(), dlg.stopbits(),
        dlg.bytesize(), dlg.timeout(), setupCmds);
}

void MainWindow::onConnected()
{
    m_connectBtn->setText(tl("btn_disconnect"));
    m_connectBtn->setEnabled(true);
    m_stopBtn->setEnabled(true);
    setSamplingState(true);
    m_localBtn->setEnabled(true);
    m_status->showMessage(
        Translations::instance().tr("status_connected",
                                    {{"port", m_connectPort}, {"baudrate", m_connectBaud}}));
}

void MainWindow::onConnectionFailed(const QString& msg)
{
    m_connectBtn->setEnabled(true);
    m_status->showMessage(tl("status_not_connected"));
    QMessageBox::critical(this, tl("dlg_conn_error_title"), msg);
}

void MainWindow::onError(const QString& msg)
{
    m_instrument->stopSampling();
    setSamplingState(false);
    m_status->showMessage(
        Translations::instance().tr("status_error", {{"msg", msg}}));
}

void MainWindow::onModeChanged(MeasMode mode)
{
    m_currentMode = mode;
    m_rangePanel->setMode(mode);
    m_display->setUnit(MODES[mode].unit);
    m_display->resetStats();
    if (m_overlay) m_overlay->setUnit(MODES[mode].unit);
    if (m_instrument->isConnected()) {
        m_instrument->sendCommand(MODES[mode].confCmd);
        sendNplc(mode);
    }
}

void MainWindow::onRangeSelected(const QString& scpiCmd)
{
    if (!m_instrument->isConnected()) return;
    if (!scpiCmd.isEmpty()) {
        m_instrument->sendCommand(scpiCmd);
    } else {
        auto it = AUTO_RANGE_CMD.find(m_currentMode);
        if (it != AUTO_RANGE_CMD.end())
            m_instrument->sendCommand(*it);
    }
}

void MainWindow::onOverload()
{
    const QString text = OVERLOAD_TEXT.value(m_currentMode, "OVLD");
    m_display->showOverload(text);
    if (m_overlay) m_overlay->showOverload(text);
}

void MainWindow::setSamplingState(bool active)
{
    m_sampling = active;
    m_stopBtn->setText(active ? tl("btn_stop") : tl("btn_resume"));
    m_stopBtn->setStyleSheet(active ? STOP_SS : BTN_SS);
}

void MainWindow::onStopResumeClicked()
{
    if (m_sampling) {
        m_instrument->stopSampling();
        setSamplingState(false);
        m_status->showMessage(tl("status_stopped"));
    } else {
        m_instrument->startSampling();
        setSamplingState(true);
        m_status->showMessage(tl("status_measuring"));
    }
}

void MainWindow::onLocalClicked()
{
    if (m_instrument->isConnected())
        m_instrument->sendCommand("SYST:LOC");
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    applyOverlay(false);
    m_instrument->stopSampling();
    if (m_instrument->isConnected())
        m_instrument->disconnectDevice();
    event->accept();
}
