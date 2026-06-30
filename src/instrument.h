/**
 * @file instrument.h
 * @brief Serial communication layer for the HP/Agilent/Keysight 34401A.
 *
 * The implementation uses a worker/facade split to keep all blocking I/O off
 * the main (GUI) thread:
 *
 *  - **InstrumentWorker** owns the QSerialPort and QTimer and lives on a
 *    dedicated QThread.  Its slots must only be invoked via
 *    Qt::QueuedConnection (or QMetaObject::invokeMethod).  Direct cross-thread
 *    calls are unsafe.
 *
 *  - **Instrument** is the public facade that lives on the main thread.  Every
 *    method it exposes is a thin wrapper that forwards the call to the worker
 *    through Qt's queued connection mechanism, making all public methods
 *    thread-safe for callers on the main thread.
 *
 * The only shared state between the two threads is
 * InstrumentWorker::connected, which is an @c std::atomic<bool> and
 * therefore safe to read from the main thread without locking.
 */
#pragma once
#include <QObject>
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QQueue>
#include <QString>
#include <QStringList>
#include <atomic>

/**
 * @brief Worker object that performs all serial I/O on a private thread.
 *
 * Instantiated and moved to a QThread by Instrument.  All public slots are
 * intended to be invoked via QMetaObject::invokeMethod with
 * Qt::QueuedConnection; calling them directly from another thread is
 * undefined behaviour.
 */
class InstrumentWorker : public QObject {
    Q_OBJECT
public:
    explicit InstrumentWorker(QObject* parent = nullptr);

    /**
     * @brief Indicates whether a serial connection is currently active.
     *
     * Written exclusively by the worker thread; may be read from any thread
     * because it is std::atomic.
     */
    std::atomic<bool> connected{false};

public slots:
    /**
     * @brief Opens the serial port, applies settings, runs @p setupCmds, then
     *        starts the measurement polling timer.
     *
     * @param port       System device path (e.g. "/dev/ttyUSB0").
     * @param baudrate   Bits per second (typically 9600 for the 34401A).
     * @param parity     Single-character parity code: "N", "E", or "O".
     * @param stopbits   Number of stop bits (1, 1.5, or 2).
     * @param bytesize   Data bits per frame (7 or 8).
     * @param timeout    Per-transaction read timeout in seconds.
     * @param setupCmds  Ordered list of SCPI commands sent after opening the
     *                   port (e.g. "SYST:REM", "CONF:VOLT:DC").
     *
     * Emits connectedSignal() on success or connectionFailed() on error.
     */
    void connectDevice(const QString& port, int baudrate, const QString& parity,
                       double stopbits, int bytesize, double timeout,
                       const QStringList& setupCmds);

    /** @brief Sends SYST:LOC, closes the port, and stops the polling timer. */
    void disconnectDevice();

    /**
     * @brief Queues a raw SCPI command to be sent before the next measurement.
     *
     * Commands are flushed in FIFO order at the start of each polling cycle.
     */
    void sendCommand(const QString& cmd);

    /** @brief Stops the polling timer without closing the serial port. */
    void stopSampling();

signals:
    /** @brief Emitted for each valid numeric reading from the instrument. */
    void measurementReceived(double value);

    /** @brief Emitted when a read/write error occurs; stops sampling. */
    void errorOccurred(const QString& msg);

    /** @brief Emitted after connectDevice() succeeds and setup commands complete. */
    void connectedSignal();

    /** @brief Emitted when connectDevice() fails to open the port or times out. */
    void connectionFailed(const QString& msg);

private slots:
    /**
     * @brief Fires on each timer tick; flushes the command queue, sends
     *        "READ?", and parses the response.
     *
     * Overrange readings (+9.9E+37) returned by the instrument are silently
     * discarded; all other numeric responses are emitted via
     * measurementReceived().
     */
    void doOneMeasurement();

private:
    QSerialPort*      m_port{nullptr};
    QTimer*           m_timer{nullptr};
    QQueue<QString>   m_cmdQueue; ///< Pending SCPI commands; drained each poll cycle
};


/**
 * @brief Thread-safe public facade for the 34401A serial interface.
 *
 * Lives on the main (GUI) thread.  Owns the QThread and InstrumentWorker.
 * All methods forward to the worker via Qt::QueuedConnection and are safe
 * to call from the main thread.
 *
 * Typical usage:
 * @code
 *   Instrument* instr = new Instrument(this);
 *   connect(instr, &Instrument::measurementReceived, display, &DisplayWidget::updateValue);
 *   instr->connectAsync("/dev/ttyUSB0", 9600, "N", 1, 8, 9.0, {"SYST:REM", "CONF:VOLT:DC"});
 * @endcode
 */
class Instrument : public QObject {
    Q_OBJECT
public:
    explicit Instrument(QObject* parent = nullptr);

    /** @brief Stops sampling, disconnects, and destroys the worker thread. */
    ~Instrument();

    /** @brief Returns @c true if the serial port is currently open. */
    bool isConnected() const { return m_worker->connected.load(); }

    /**
     * @brief Asynchronously opens the serial port and begins measurements.
     *
     * Returns immediately; the result is reported via connected() or
     * connectionFailed().
     *
     * @param setupCmds  SCPI commands sent in order after the port opens.
     *                   "SYST:REM" should always be first to assert remote control.
     */
    void connectAsync(const QString& port, int baudrate, const QString& parity,
                      double stopbits, int bytesize, double timeout,
                      const QStringList& setupCmds);

    /** @brief Closes the serial port and returns the instrument to local control. */
    void disconnectDevice();

    /**
     * @brief Queues a raw SCPI command string for delivery on the next poll cycle.
     *
     * Safe to call while sampling is active; the command is delivered before
     * the following READ?.
     */
    void sendCommand(const QString& cmd);

    /** @brief Pauses measurement polling without closing the serial connection. */
    void stopSampling();

    /**
     * @brief Returns the list of available serial port device paths.
     *
     * Wraps QSerialPortInfo::availablePorts().  Returns strings in the form
     * "/dev/ttyUSBn" on Linux or "COMn" on Windows.
     */
    static QStringList listPorts();

signals:
    /** @brief Re-emitted from InstrumentWorker::measurementReceived. */
    void measurementReceived(double value);

    /** @brief Re-emitted from InstrumentWorker::errorOccurred. */
    void errorOccurred(const QString& msg);

    /** @brief Emitted when the connection handshake completes successfully. */
    void connected();

    /** @brief Emitted when the connection attempt fails. */
    void connectionFailed(const QString& msg);

private:
    QThread*          m_thread;
    InstrumentWorker* m_worker;
};
