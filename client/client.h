#ifndef CLIENT_H
#define CLIENT_H
#include <QTcpSocket>
class Client :public QObject
{
    Q_OBJECT
public:
    Client();
    void connectToServer();
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
