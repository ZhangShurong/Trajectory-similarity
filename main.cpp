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
//    m.setWindowOpacity(1);
    m.setWindowFlags(Qt::Window);
//    m.setAttribute(Qt::WA_TranslucentBackground);
    m.setDb(&db);
    m.showFullScreen();
    //m.show();

    a.exec();
    return 0;
}
