#pragma once
#include <QDialog>
#include <QString>

class QComboBox;
class QCheckBox;
class QPushButton;

class SetupDialog : public QDialog {
    Q_OBJECT
public:
    explicit SetupDialog(QWidget* parent = nullptr);

private:
    void buildUi();
    QWidget* buildLanguageTab();
    QWidget* buildInterfaceTab();
    QWidget* buildMeasurementTab();
    QWidget* buildDisplayTab();
    QWidget* buildOverlayTab();
    void loadFromConfig();
    void saveAndAccept();
    void pickOverlayColor();
    void onLangChanged();

    QString m_originalLang;
    QString m_overlayColor;

    QComboBox* m_langCombo{nullptr};
    QComboBox* m_portCombo{nullptr};
    QComboBox* m_baudCombo{nullptr};
    QComboBox* m_parityCombo{nullptr};
    QComboBox* m_stopbitsCombo{nullptr};
    QComboBox* m_bytesizeCombo{nullptr};
    QComboBox* m_timeoutCombo{nullptr};
    QComboBox* m_samplingCombo{nullptr};
    QComboBox* m_nplcCombo{nullptr};
    QCheckBox* m_autozeroChk{nullptr};
    QComboBox* m_decimalsCombo{nullptr};
    QCheckBox* m_showStatsChk{nullptr};
    QCheckBox* m_overlayChk{nullptr};
    QPushButton* m_colorBtn{nullptr};
    QComboBox* m_overlayFontCombo{nullptr};
    QComboBox* m_overlaySizeCombo{nullptr};
};
