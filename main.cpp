#include "TextToImg.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    translator.load("://widgets_qm");
    a.installTranslator(&translator);
    TextToImg w;
    w.show();

    return a.exec();
}
