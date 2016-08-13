#ifndef CLIENT_H
#define CLIENT_H
#include <QTcpSocket>
#include "sequence.h"
class Client :public QObject
{
    Q_OBJECT
public:
    Client();
    void connectToServer();
    void search();
    void insert();
    Sequence sequence;
    std::vector<Sequence> sequences;
    ~Client();
private slots:
    void sendRequest();
    void stop();
    void connectionClosedByServer();
    void error();
    void getData();
private:
    void closeConnection();
    QTcpSocket tcpSocket;
    quint16 nextBlockSize;

};

#endif // CLIENT_H
