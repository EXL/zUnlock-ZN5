#include "Widget.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator appTranslator;
    appTranslator.load("zUnlock-ZN5_" + QLocale::system().name(), "://l10n/");
    a.installTranslator(&appTranslator);

    Widget w;
    w.show();

    return a.exec();
}
