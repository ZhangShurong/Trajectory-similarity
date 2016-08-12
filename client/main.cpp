#include <QCoreApplication>
#include "client.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Client client;
    client.connectToServer();
    return a.exec();
}
