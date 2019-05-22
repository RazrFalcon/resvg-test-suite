#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    a.setOrganizationName("resvg");
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);

    MainWindow w;
    w.show();

    return a.exec();
}
