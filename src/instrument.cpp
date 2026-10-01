/**
 * @file instrument.cpp
 * @brief InstrumentWorker and Instrument implementation.
 *
 * See instrument.h for the threading model and design rationale.
 */
#include "instrument.h"
#include "config.h"
#include <QThread>
#include <QElapsedTimer>
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

    // The 34401A reconfigures its front end after a CONF: command and is
    // busy for up to a second; commands (incl. READ?) arriving in that
    // window are rejected with an error.  1 s lets the instrument settle;
    // clearing the buffer discards anything that arrived during that window.
    QThread::msleep(1000);
    m_port->clear();

    connected.store(true);
    emit connectedSignal();

    startSampling(); // first measurement fires immediately

    // timeoutMs is read per-measurement from AppConfig so it reflects any
    // changes made in the Setup dialog without requiring a reconnect.
    Q_UNUSED(timeoutMs)
}

void InstrumentWorker::doOneMeasurement()
{
    if (!m_port || !m_port->isOpen()) return;
    m_inCycle = true;

    // Discard stale bytes so leftover data from a previous cycle cannot
    // corrupt this cycle's response.
    m_port->clear();
    m_rxBuffer.clear();

    // Process pending commands
    while (!m_cmdQueue.isEmpty()) {
        QString cmd = m_cmdQueue.dequeue();
        m_port->write((cmd + "\r\n").toUtf8());
        m_port->waitForBytesWritten(4000);
        // The 34401A rejects commands received while it reconfigures its
        // front end after a CONF: (error LED + error beep, and the error
        // beep cannot be disabled with SYST:BEEP:STAT).  Settle after every
        // command so the next one lands on an idle instrument.  A CONF:
        // that changes the measurement function reconfigures the front end
        // for up to a couple of seconds; a range-only CONF: on the current
        // function settles within a second; plain parameter commands
        // (NPLC, zero auto) settle quickly.
        bool isConf = cmd.startsWith("CONF:") || cmd.startsWith("MEAS:");
        if (isConf) {
            QString func = cmd.section(' ', 1, 1); // e.g. "VOLT:DC" from "CONF:VOLT:DC 10,DEF"
            bool funcChange = !m_lastFunc.isEmpty() && m_lastFunc != func;
            m_lastFunc = func;
            QThread::msleep(funcChange ? 2000 : 1000);
        } else {
            QThread::msleep(100);
        }
        m_port->clear();
    }

    int timeoutMs = static_cast<int>(AppConfig::instance().serialTimeout().toDouble() * 1000);
    // The 34401A rejects a READ? that arrives while a measurement is in
    // progress (error LED + beep).  A single measurement takes up to ~1.2 s
    // (frequency/period) and AC measurements can be slower, so the patience
    // window must exceed the longest measurement time; a shorter window
    // makes every retry land in the busy window and beep once per cycle.
    // Paced instead: send one READ?, wait up to READ_SETTLE_MS for the
    // response line, retry until the full timeout.  In steady state the
    // poll rate settles at the measurement rate, so every READ? lands on an
    // idle instrument.
    const int READ_SETTLE_MS = 2500;
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
    QByteArray line;
    bool gotLine = false;
    while (!gotLine && elapsedTimer.elapsed() < timeoutMs) {
        m_port->clear();
        m_rxBuffer.clear();
        m_port->write("READ?\r\n");
        m_port->waitForBytesWritten(4000);

        int waited = 0;
        while (!gotLine && waited < READ_SETTLE_MS &&
               elapsedTimer.elapsed() < timeoutMs) {
            int chunk = qMin(10, qMin(READ_SETTLE_MS - waited,
                timeoutMs - static_cast<int>(elapsedTimer.elapsed())));
            if (chunk <= 0) break;
            if (m_port->waitForReadyRead(chunk))
                m_rxBuffer.append(m_port->readAll());
            waited += chunk;
            int nl = m_rxBuffer.indexOf('\n');
            if (nl >= 0) {
                line = m_rxBuffer.left(nl);
                m_rxBuffer.remove(0, nl + 1);
                gotLine = true;
            }
            // If timer was stopped externally while we were waiting, bail out
            if (!m_timer || !m_port || !m_port->isOpen()) break;
        }

        if (!gotLine) {
            // No response within the patience window: the READ? was most
            // likely rejected (busy instrument).  Read the instrument's
            // error queue so the actual cause is visible in the status bar.
            QString err = queryError();
            if (!err.isEmpty() && err != "0")
                emit statusMessage("34401A Fehler nach READ?: " + err);
        }
    }

    if (gotLine) {
        bool ok;
        double val = line.trimmed().toDouble(&ok);
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

    m_inCycle = false;
    if (m_timer && m_port && m_port->isOpen())
        m_timer->start(AppConfig::instance().samplingMs());
}

QString InstrumentWorker::queryError()
{
    if (!m_port || !m_port->isOpen()) return QString();
    m_port->clear();
    m_rxBuffer.clear();
    m_port->write("SYST:ERR?\r\n");
    m_port->waitForBytesWritten(4000);

    QElapsedTimer t;
    t.start();
    while (t.elapsed() < 1000) {
        if (m_port->waitForReadyRead(30))
            m_rxBuffer.append(m_port->readAll());
        int nl = m_rxBuffer.indexOf('\n');
        if (nl >= 0)
            return m_rxBuffer.left(nl).trimmed();
    }
    return QString();
}

void InstrumentWorker::sendCommand(const QString& cmd)
{
    m_cmdQueue.enqueue(cmd);
    // If a cycle is not running, pull the next tick forward so the command
    // is delivered without waiting for the full sampling interval.
    if (!m_inCycle && m_timer && m_timer->isActive())
        m_timer->start(50);
}

void InstrumentWorker::stopSampling()
{
    if (m_timer) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }
}

void InstrumentWorker::startSampling()
{
    if (!m_port || !m_port->isOpen()) return;
    if (m_timer) return; // already sampling

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setTimerType(Qt::PreciseTimer);
    connect(m_timer, &QTimer::timeout, this, &InstrumentWorker::doOneMeasurement);
    m_timer->start(0); // resume immediately
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
    connect(m_worker, &InstrumentWorker::statusMessage,
            this,     &Instrument::statusMessage);

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

void Instrument::startSampling()
{
    QMetaObject::invokeMethod(m_worker, [this]() { m_worker->startSampling(); },
                              Qt::QueuedConnection);
}

QStringList Instrument::listPorts()
{
    QStringList result;
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts())
        result << info.systemLocation();
    return result;
}
