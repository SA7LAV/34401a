#pragma once
#include <QDialog>

class QComboBox;
class QLabel;

class ConnectionDialog : public QDialog {
    Q_OBJECT
public:
    explicit ConnectionDialog(QWidget* parent = nullptr);

    QString port()     const;
    int     baudrate() const;
    QString parity()   const;
    double  stopbits() const;
    int     bytesize() const;
    double  timeout()  const;

private:
    void buildUi();
    void loadConfig();
    void saveAndAccept();

    QLabel*   m_portLbl{nullptr};
    QLabel*   m_baudLbl{nullptr};
    QLabel*   m_parityLbl{nullptr};
    QLabel*   m_stopbitsLbl{nullptr};
    QLabel*   m_bytesizeLbl{nullptr};
    QLabel*   m_timeoutLbl{nullptr};
    QComboBox* m_portCombo{nullptr};
    QComboBox* m_baudCombo{nullptr};
    QComboBox* m_parityCombo{nullptr};
    QComboBox* m_stopbitsCombo{nullptr};
    QComboBox* m_bytesizeCombo{nullptr};
    QComboBox* m_timeoutCombo{nullptr};
};
