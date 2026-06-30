#include "connectiondialog.h"
#include "../translations.h"
#include "../config.h"
#include "../instrument.h"
#include "dialogstyle.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QLabel>
#include <QDialogButtonBox>

ConnectionDialog::ConnectionDialog(QWidget* parent) : QDialog(parent)
{
    setModal(true);
    setStyleSheet(dialogStyle());
    buildUi();
    loadConfig();
    setWindowTitle(tl("dlg_conn_title"));
}

void ConnectionDialog::buildUi()
{
    auto* layout = new QVBoxLayout(this);
    auto* form   = new QFormLayout;

    m_portLbl  = new QLabel(tl("lbl_port"));
    m_portCombo = new QComboBox;
    m_portCombo->addItems(Instrument::listPorts());
    form->addRow(m_portLbl, m_portCombo);

    m_baudLbl  = new QLabel(tl("lbl_baudrate"));
    m_baudCombo = new QComboBox;
    m_baudCombo->addItems({"300","600","1200","2400","4800","9600","19200","38400","57600","115200"});
    m_baudCombo->setCurrentText("9600");
    form->addRow(m_baudLbl, m_baudCombo);

    m_parityLbl  = new QLabel(tl("lbl_parity"));
    m_parityCombo = new QComboBox;
    m_parityCombo->addItem(tl("parity_none"), "N");
    m_parityCombo->addItem(tl("parity_even"), "E");
    m_parityCombo->addItem(tl("parity_odd"),  "O");
    form->addRow(m_parityLbl, m_parityCombo);

    m_stopbitsLbl  = new QLabel(tl("lbl_stopbits"));
    m_stopbitsCombo = new QComboBox;
    m_stopbitsCombo->addItems({"1", "1.5", "2"});
    form->addRow(m_stopbitsLbl, m_stopbitsCombo);

    m_bytesizeLbl  = new QLabel(tl("lbl_bytesize"));
    m_bytesizeCombo = new QComboBox;
    m_bytesizeCombo->addItems({"7", "8"});
    form->addRow(m_bytesizeLbl, m_bytesizeCombo);

    m_timeoutLbl  = new QLabel(tl("lbl_timeout"));
    m_timeoutCombo = new QComboBox;
    m_timeoutCombo->addItems({"1.0", "2.0", "5.0", "9.0", "15.0"});
    form->addRow(m_timeoutLbl, m_timeoutCombo);

    layout->addLayout(form);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &ConnectionDialog::saveAndAccept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

void ConnectionDialog::loadConfig()
{
    auto& cfg = AppConfig::instance();
    QStringList ports = Instrument::listPorts();
    if (ports.contains(cfg.serialPort()))
        m_portCombo->setCurrentText(cfg.serialPort());
    m_baudCombo->setCurrentText(cfg.serialBaudrate());
    int parityIdx = m_parityCombo->findData(cfg.serialParity());
    m_parityCombo->setCurrentIndex(parityIdx >= 0 ? parityIdx : 0);
    m_stopbitsCombo->setCurrentText(cfg.serialStopbits());
    m_bytesizeCombo->setCurrentText(cfg.serialBytesize());
    m_timeoutCombo->setCurrentText(cfg.serialTimeout());
}

void ConnectionDialog::saveAndAccept()
{
    auto& cfg = AppConfig::instance();
    cfg.set("serial", "port",     m_portCombo->currentText());
    cfg.set("serial", "baudrate", m_baudCombo->currentText());
    cfg.set("serial", "parity",   m_parityCombo->currentData().toString());
    cfg.set("serial", "stopbits", m_stopbitsCombo->currentText());
    cfg.set("serial", "bytesize", m_bytesizeCombo->currentText());
    cfg.set("serial", "timeout",  m_timeoutCombo->currentText());
    cfg.save();
    accept();
}

QString ConnectionDialog::port()     const { return m_portCombo->currentText(); }
int     ConnectionDialog::baudrate() const { return m_baudCombo->currentText().toInt(); }
QString ConnectionDialog::parity()   const { return m_parityCombo->currentData().toString(); }
double  ConnectionDialog::stopbits() const { return m_stopbitsCombo->currentText().toDouble(); }
int     ConnectionDialog::bytesize() const { return m_bytesizeCombo->currentText().toInt(); }
double  ConnectionDialog::timeout()  const { return m_timeoutCombo->currentText().toDouble(); }
