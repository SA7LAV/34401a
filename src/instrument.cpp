/**
 * @file instrument.cpp
 * @brief InstrumentWorker and Instrument implementation.
 *
 * See instrument.h for the threading model and design rationale.
 */
#include "instrument.h"
#include "config.h"
#include <QThread>
#include <cmath>

// ──────────────────────────────── Worker ─────────────────────────────────────

InstrumentWorker::InstrumentWorker(QObject* parent) : QObject(parent) {}

void InstrumentWorker::connectDevice(const QString& port, int baudrate,
                                     const QString& parity, double stopbits,
                                     int bytesize, double timeout,
                                     const QStringList& setupCmds)
{
    auto parityMap = [](const QString& p) -> QSerialPort::Parity {
        if (p == "E") return QSerialPort::EvenParity;
        if (p == "O") return QSerialPort::OddParity;
        return QSerialPort::NoParity;
    };
    auto stopMap = [](double s) -> QSerialPort::StopBits {
        if (s == 2.0) return QSerialPort::TwoStop;
        if (s == 1.5) return QSerialPort::OneAndHalfStop;
        return QSerialPort::OneStop;
    };
    auto dataMap = [](int b) -> QSerialPort::DataBits {
        return b == 7 ? QSerialPort::Data7 : QSerialPort::Data8;
    };

    m_port = new QSerialPort(this);
    m_port->setPortName(port);
    m_port->setBaudRate(baudrate);
    m_port->setParity(parityMap(parity));
    m_port->setStopBits(stopMap(stopbits));
    m_port->setDataBits(dataMap(bytesize));
    m_port->setFlowControl(QSerialPort::NoFlowControl);

    if (!m_port->open(QIODevice::ReadWrite)) {
        QString msg = m_port->errorString();
        delete m_port;
        m_port = nullptr;
        emit connectionFailed(msg);
        return;
    }

    m_port->clear();
    int timeoutMs = static_cast<int>(timeout * 1000);

    for (const QString& cmd : setupCmds) {
        m_port->write((cmd + "\r\n").toUtf8());
        m_port->waitForBytesWritten(4000);
    }

    // The 34401A emits a short beep and is briefly busy after a CONF: command.
    // 300 ms is sufficient to let the instrument settle; clearing the buffer
    // discards any partial response or echo that arrived during that window.
    QThread::msleep(300);
    m_port->clear();

    connected.store(true);
    emit connectedSignal();

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &InstrumentWorker::doOneMeasurement);
    m_timer->start(0); // first measurement immediately

    // timeoutMs is read per-measurement from AppConfig so it reflects any
    // changes made in the Setup dialog without requiring a reconnect.
    Q_UNUSED(timeoutMs)
}

void InstrumentWorker::doOneMeasurement()
{
    if (!m_port || !m_port->isOpen()) return;

    // Process pending commands
    bool hadConf = false;
    while (!m_cmdQueue.isEmpty()) {
        QString cmd = m_cmdQueue.dequeue();
        m_port->write((cmd + "\r\n").toUtf8());
        m_port->waitForBytesWritten(4000);
        if (cmd.startsWith("CONF:") || cmd.startsWith("MEAS:"))
            hadConf = true;
    }

    if (hadConf) {
        QThread::msleep(300);
        m_port->clear();
    }

    m_port->write("READ?\r\n");

    int timeoutMs = static_cast<int>(AppConfig::instance().serialTimeout().toDouble() * 1000);
    // Poll in short bursts so we can stop without a long wait
    int elapsed = 0;
    bool gotData = false;
    while (elapsed < timeoutMs) {
        int wait = qMin(200, timeoutMs - elapsed);
        if (m_port->waitForReadyRead(wait)) {
            gotData = true;
            break;
        }
        elapsed += wait;
        // If timer was stopped externally while we were waiting, bail out
        if (!m_timer || !m_port || !m_port->isOpen()) return;
    }

    if (gotData) {
        QByteArray resp = m_port->readLine().trimmed();
        if (!resp.isEmpty()) {
            bool ok;
            double val = resp.toDouble(&ok);
            if (ok) {
                // +9.9E+37 is the SCPI overrange sentinel returned by the 34401A
                // when the input exceeds the selected measurement range.  Report
                // it as an explicit overload rather than propagating a meaningless
                // large number (or silently freezing on the last valid reading).
                if (std::abs(val) >= OVERLOAD_SENTINEL)
                    emit overloadDetected();
                else
                    emit measurementReceived(val);
            }
        }
    }

    if (m_timer && m_port && m_port->isOpen())
        m_timer->start(AppConfig::instance().samplingMs());
}

void InstrumentWorker::sendCommand(const QString& cmd)
{
    m_cmdQueue.enqueue(cmd);
}

void InstrumentWorker::stopSampling()
{
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }
}

void InstrumentWorker::disconnectDevice()
{
    stopSampling();
    if (m_port && m_port->isOpen()) {
        // SYST:LOC releases the instrument from remote control so the front
        // panel becomes active again after the application disconnects.
        m_port->write("SYST:LOC\r\n");
        m_port->waitForBytesWritten(2000);
        m_port->close();
    }
    delete m_port;
    m_port = nullptr;
    connected.store(false);
}

// ──────────────────────────────── Facade ─────────────────────────────────────

Instrument::Instrument(QObject* parent) : QObject(parent)
{
    m_worker = new InstrumentWorker;
    m_thread = new QThread(this);
    m_worker->moveToThread(m_thread);

    connect(m_worker, &InstrumentWorker::measurementReceived,
            this,     &Instrument::measurementReceived);
    connect(m_worker, &InstrumentWorker::overloadDetected,
            this,     &Instrument::overloadDetected);
    connect(m_worker, &InstrumentWorker::errorOccurred,
            this,     &Instrument::errorOccurred);
    connect(m_worker, &InstrumentWorker::connectedSignal,
            this,     &Instrument::connected);
    connect(m_worker, &InstrumentWorker::connectionFailed,
            this,     &Instrument::connectionFailed);

    connect(m_thread, &QThread::finished, m_worker, &QObject::deleteLater);
    m_thread->start();
}

Instrument::~Instrument()
{
    QMetaObject::invokeMethod(m_worker, [this]() { m_worker->disconnectDevice(); },
                              Qt::BlockingQueuedConnection);
    m_thread->quit();
    m_thread->wait(3000);
}

void Instrument::connectAsync(const QString& port, int baudrate, const QString& parity,
                              double stopbits, int bytesize, double timeout,
                              const QStringList& setupCmds)
{
    QMetaObject::invokeMethod(m_worker, [=, this]() {
        m_worker->connectDevice(port, baudrate, parity, stopbits, bytesize, timeout, setupCmds);
    }, Qt::QueuedConnection);
}

void Instrument::disconnectDevice()
{
    QMetaObject::invokeMethod(m_worker, [this]() { m_worker->disconnectDevice(); },
                              Qt::QueuedConnection);
}

void Instrument::sendCommand(const QString& cmd)
{
    QMetaObject::invokeMethod(m_worker, [=, this]() { m_worker->sendCommand(cmd); },
                              Qt::QueuedConnection);
}

void Instrument::stopSampling()
{
    QMetaObject::invokeMethod(m_worker, [this]() { m_worker->stopSampling(); },
                              Qt::QueuedConnection);
}

QStringList Instrument::listPorts()
{
    QStringList result;
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts())
        result << info.systemLocation();
    return result;
}
