#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
class Server :public QTcpServer
{
public:
    Server();
    ~Server();
private:
    void incomingConnection(int socketId);
};

#endif // SERVER_H
