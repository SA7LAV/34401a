#include <QApplication>
#include "config.h"
#include "translations.h"
#include "ui/mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    auto& cfg = AppConfig::instance();
    Translations::instance().setLanguage(cfg.language());

    MainWindow window;
    window.show();

    return app.exec();
}
