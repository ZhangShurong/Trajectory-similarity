#include <QCoreApplication>
#include <iostream>
#include "server.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server server;
    if(!server.listen(QHostAddress::Any, 10086))
    {
        std::cerr << "Failed to bind to port \n";
        return 1;
    }
    else
    {
        std::cout << "Runing\n";
    }
    return a.exec();
}
