/**
 * @file connectiondialog.h
 * @brief Modal dialog for entering serial connection parameters.
 *
 * Pre-populates all fields from AppConfig and writes the values back on
 * acceptance.  The dialog does not open the serial port; it merely collects
 * and validates the parameters so MainWindow can pass them to
 * Instrument::connectAsync().
 */
#pragma once
#include <QDialog>

class QComboBox;
class QLabel;

/**
 * @brief Serial parameter entry dialog shown before each connection attempt.
 *
 * After exec() returns QDialog::Accepted the caller can read the selected
 * parameters via the accessor methods.  The values are also saved to
 * AppConfig so they are pre-filled on the next invocation.
 */
class ConnectionDialog : public QDialog {
    Q_OBJECT
public:
    explicit ConnectionDialog(QWidget* parent = nullptr);

    /** @brief Returns the selected device path (e.g. "/dev/ttyUSB0"). */
    QString port()     const;

    /** @brief Returns the selected baud rate (e.g. 9600). */
    int     baudrate() const;

    /**
     * @brief Returns the selected parity as a single-character code.
     * @return "N" (none), "E" (even), or "O" (odd).
     */
    QString parity()   const;

    /** @brief Returns the number of stop bits (1.0, 1.5, or 2.0). */
    double  stopbits() const;

    /** @brief Returns the number of data bits per frame (7 or 8). */
    int     bytesize() const;

    /** @brief Returns the per-transaction read timeout in seconds. */
    double  timeout()  const;

private:
    /** @brief Constructs the form layout and OK/Cancel buttons. */
    void buildUi();

    /** @brief Loads saved parameters from AppConfig into the form controls. */
    void loadConfig();

    /** @brief Writes the current form values to AppConfig and accepts the dialog. */
    void saveAndAccept();

    QLabel*    m_portLbl{nullptr};
    QLabel*    m_baudLbl{nullptr};
    QLabel*    m_parityLbl{nullptr};
    QLabel*    m_stopbitsLbl{nullptr};
    QLabel*    m_bytesizeLbl{nullptr};
    QLabel*    m_timeoutLbl{nullptr};
    QComboBox* m_portCombo{nullptr};
    QComboBox* m_baudCombo{nullptr};
    QComboBox* m_parityCombo{nullptr};
    QComboBox* m_stopbitsCombo{nullptr};
    QComboBox* m_bytesizeCombo{nullptr};
    QComboBox* m_timeoutCombo{nullptr};
};
