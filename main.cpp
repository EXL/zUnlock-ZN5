#include "Widget.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator appTranslator;
    QTranslator qtTranslator;
    QString qmPath = "://l10n/";
    appTranslator.load("zUnlock-ZN5_" + QLocale::system().name(), qmPath);
    qtTranslator.load("qt_"  + QLocale::system().name(), qmPath);
    a.installTranslator(&appTranslator);
    a.installTranslator(&qtTranslator);

    Widget w;
    w.show();

    return a.exec();
}
