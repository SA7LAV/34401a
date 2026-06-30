/**
 * @file main.cpp
 * @brief Application entry point for the HP/Agilent/Keysight 34401A Multimeter GUI.
 *
 * Initialises the Qt application, loads the persisted language preference, and
 * shows the main window.  The Fusion style is used because it renders correctly
 * on both X11 and Wayland without additional platform-specific tweaks.
 */
#include <QApplication>
#include "config.h"
#include "translations.h"
#include "ui/mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    // Fusion gives a consistent cross-platform look; the custom stylesheet
    // applied in each widget then overrides colours for the dark theme.
    app.setStyle("Fusion");

    auto& cfg = AppConfig::instance();
    Translations::instance().setLanguage(cfg.language());

    MainWindow window;
    window.show();

    return app.exec();
}
