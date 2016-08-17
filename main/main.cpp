#include <QApplication>
#include <QFile>
#include "mainwindow.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //DataBase db;
    MainWindow m;
    m.setWindowFlags(Qt::Window);
    //m.setDb(&db);
    m.showFullScreen();
    a.exec();
    return 0;
}
