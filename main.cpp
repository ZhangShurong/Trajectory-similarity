#include <QApplication>
#include <QFile>
#include "mainwindow.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DataBase db;
//    mainWin wi;
//    wi.setDb(&db);
//    wi.resize(900, 671);
//    wi.show();
    MainWindow m;
    m.setDb(&db);
    m.show();

    a.exec();
    return 0;
}
