#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator;
    if (qtTranslator.load("qtbase_ru.qm")) {
        a.installTranslator(&qtTranslator);
    }

    MainWindow w;
    w.show();

    return a.exec();
}