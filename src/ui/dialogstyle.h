/**
 * @file dialogstyle.h
 * @brief Shared Qt stylesheet for all modal dialogs.
 *
 * Returns a CSS string that applies the application's dark blue theme to
 * QDialog, QTabWidget, QComboBox, QCheckBox, and QDialogButtonBox.  Include
 * this header and call @c setStyleSheet(dialogStyle()) in any new dialog's
 * constructor to keep the visual style consistent.
 *
 * Colour palette used:
 *  - Background:  #12182E (darkest), #0E1428 (dark), #1A2040 (medium)
 *  - Border:      #2A3560
 *  - Text:        #D0E4F4 (primary), #8AAAC8 (secondary)
 *  - Accent/active: #00C0FF
 *  - Hover:       #203060
 */
#pragma once
#include <QString>

/**
 * @brief Returns the shared dialog stylesheet string.
 *
 * The stylesheet is generated inline (no resource file dependency) so it can
 * be called before the Qt resource system is initialised.
 */
inline QString dialogStyle()
{
    return R"(
QDialog {
    background-color: #12182E;
}
QTabWidget::pane {
    background-color: #1A2040;
    border: 1px solid #2A3560;
}
QTabBar::tab {
    background-color: #0E1428;
    color: #8AAAC8;
    padding: 6px 16px;
    border: 1px solid #2A3560;
    border-bottom: none;
}
QTabBar::tab:selected {
    background-color: #1A2040;
    color: #00C0FF;
}
QWidget {
    background-color: #1A2040;
    color: #D0E4F4;
}
QLabel {
    color: #D0E4F4;
    background: transparent;
}
QComboBox {
    background-color: #0E1428;
    color: #D0E4F4;
    border: 1px solid #2A3560;
    border-radius: 3px;
    padding: 3px 8px;
    min-width: 120px;
}
QComboBox::drop-down {
    border: none;
}
QComboBox QAbstractItemView {
    background-color: #0E1428;
    color: #D0E4F4;
    selection-background-color: #203060;
}
QCheckBox {
    color: #D0E4F4;
    background: transparent;
}
QCheckBox::indicator {
    width: 16px;
    height: 16px;
    border: 2px solid #4A6080;
    border-radius: 3px;
    background-color: #0E1428;
}
QCheckBox::indicator:checked {
    background-color: #0E1428;
    border-color: #00C0FF;
    image: url(:/assets/checkmark.svg);
}
QCheckBox::indicator:hover {
    border-color: #00C0FF;
}
QDialogButtonBox QPushButton {
    background-color: #1A2040;
    color: #00C0FF;
    border: 1px solid #00C0FF;
    border-radius: 4px;
    padding: 5px 20px;
}
QDialogButtonBox QPushButton:hover {
    background-color: #203060;
}
)";
}
