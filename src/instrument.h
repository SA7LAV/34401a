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

class InstrumentWorker : public QObject {
    Q_OBJECT
public:
    explicit InstrumentWorker(QObject* parent = nullptr);

    std::atomic<bool> connected{false};

public slots:
    void connectDevice(const QString& port, int baudrate, const QString& parity,
                       double stopbits, int bytesize, double timeout,
                       const QStringList& setupCmds);
    void disconnectDevice();
    void sendCommand(const QString& cmd);
    void stopSampling();

signals:
    void measurementReceived(double value);
    void errorOccurred(const QString& msg);
    void connectedSignal();
    void connectionFailed(const QString& msg);

private slots:
    void doOneMeasurement();

private:
    QSerialPort* m_port{nullptr};
    QTimer* m_timer{nullptr};
    QQueue<QString> m_cmdQueue;
};


class Instrument : public QObject {
    Q_OBJECT
public:
    explicit Instrument(QObject* parent = nullptr);
    ~Instrument();

    bool isConnected() const { return m_worker->connected.load(); }

    void connectAsync(const QString& port, int baudrate, const QString& parity,
                      double stopbits, int bytesize, double timeout,
                      const QStringList& setupCmds);
    void disconnectDevice();
    void sendCommand(const QString& cmd);
    void stopSampling();

    static QStringList listPorts();

signals:
    void measurementReceived(double value);
    void errorOccurred(const QString& msg);
    void connected();
    void connectionFailed(const QString& msg);

private:
    QThread* m_thread;
    InstrumentWorker* m_worker;
};
