/**
 * @file setupdialog.h
 * @brief Modal settings dialog with five configuration tabs.
 *
 * The dialog supports a **live language preview**: changing the Language
 * combo retranslates the entire UI immediately via the Translations singleton.
 * If the user clicks Cancel the original language is restored.
 *
 * Tabs:
 *  - **Language**    – UI language (Deutsch / English / Svenska)
 *  - **Interface**   – Serial port parameters
 *  - **Measurement** – Sampling rate, NPLC, Auto-Zero
 *  - **Display**     – Decimal places, MIN/MAX/AVG visibility
 *  - **Overlay**     – OBS overlay enable, font, colour, size
 */
#pragma once
#include <QDialog>
#include <QString>

class QComboBox;
class QCheckBox;
class QPushButton;

/**
 * @brief Settings dialog; reads from and writes to AppConfig.
 *
 * Config is loaded into the widgets on construction.  Clicking OK calls
 * saveAndAccept() which writes every setting and calls AppConfig::save().
 * Clicking Cancel reverts any live language preview without saving.
 */
class SetupDialog : public QDialog {
    Q_OBJECT
public:
    explicit SetupDialog(QWidget* parent = nullptr);

private:
    /** @brief Builds the tab widget and OK/Cancel button box. */
    void buildUi();

    /** @brief Builds and returns the Language tab widget. */
    QWidget* buildLanguageTab();

    /** @brief Builds and returns the Interface (serial) tab widget. */
    QWidget* buildInterfaceTab();

    /** @brief Builds and returns the Measurement tab widget. */
    QWidget* buildMeasurementTab();

    /** @brief Builds and returns the Display tab widget. */
    QWidget* buildDisplayTab();

    /** @brief Builds and returns the Overlay tab widget. */
    QWidget* buildOverlayTab();

    /** @brief Populates all controls from AppConfig. */
    void loadFromConfig();

    /**
     * @brief Writes all control values to AppConfig, calls save(), and
     *        closes the dialog with Accepted result.
     */
    void saveAndAccept();

    /**
     * @brief Opens a colour picker and updates the colour swatch button.
     *
     * The selected colour is buffered in m_overlayColor; it is not written to
     * AppConfig until saveAndAccept() is called.
     */
    void pickOverlayColor();

    /**
     * @brief Applies the selected language immediately for a live preview.
     *
     * Triggered on every combo-box change; reverted on Cancel via m_originalLang.
     */
    void onLangChanged();

    QString m_originalLang; ///< Language active when the dialog opened; restored on Cancel
    QString m_overlayColor; ///< Buffered overlay colour (hex string, e.g. "#00C0FF")

    QComboBox*   m_langCombo{nullptr};
    QComboBox*   m_portCombo{nullptr};
    QComboBox*   m_baudCombo{nullptr};
    QComboBox*   m_parityCombo{nullptr};
    QComboBox*   m_stopbitsCombo{nullptr};
    QComboBox*   m_bytesizeCombo{nullptr};
    QComboBox*   m_timeoutCombo{nullptr};
    QComboBox*   m_samplingCombo{nullptr};
    QComboBox*   m_nplcCombo{nullptr};
    QCheckBox*   m_autozeroChk{nullptr};
    QComboBox*   m_decimalsCombo{nullptr};
    QCheckBox*   m_showStatsChk{nullptr};
    QCheckBox*   m_overlayChk{nullptr};
    QPushButton* m_colorBtn{nullptr};     ///< Colour swatch; click to open colour picker
    QComboBox*   m_overlayFontCombo{nullptr};
    QComboBox*   m_overlaySizeCombo{nullptr};
};
