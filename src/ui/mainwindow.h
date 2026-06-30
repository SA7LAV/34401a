#pragma once
#include <QMainWindow>
#include "../models.h"

class Instrument;
class DisplayWidget;
class ModePanel;
class RangePanel;
class OverlayWindow;
class QLabel;
class QPushButton;
class QStatusBar;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void buildUi();
    void wireSignals();
    void applyOverlay(bool enabled);
    void sendNplc(MeasMode mode);
    void retranslateUi();

    void onSetupClicked();
    void onConnectClicked();
    void onStopClicked();
    void onLocalClicked();
    void onModeChanged(MeasMode mode);
    void onRangeSelected(const QString& scpiCmd);
    void onConnected();
    void onConnectionFailed(const QString& msg);
    void onError(const QString& msg);

    Instrument*    m_instrument{nullptr};
    DisplayWidget* m_display{nullptr};
    ModePanel*     m_modePanel{nullptr};
    RangePanel*    m_rangePanel{nullptr};
    OverlayWindow* m_overlay{nullptr};
    QLabel*        m_lblFunction{nullptr};
    QLabel*        m_lblRange{nullptr};
    QPushButton*   m_setupBtn{nullptr};
    QPushButton*   m_connectBtn{nullptr};
    QPushButton*   m_stopBtn{nullptr};
    QPushButton*   m_localBtn{nullptr};
    QStatusBar*    m_status{nullptr};

    MeasMode m_currentMode{MeasMode::VDC};
    QString  m_connectPort;
    QString  m_connectBaud;
};
