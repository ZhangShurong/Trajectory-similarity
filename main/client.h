#ifndef CLIENT_H
#define CLIENT_H
#include <QTcpSocket>
#include <QHostAddress>
#include <QObject>
#include <QDataStream>
#include "sequence.h"
class Client:public QObject
{
    Q_OBJECT
public:
    QString msg;
    Sequence sequence;
    std::vector<Sequence> sequences;

    explicit Client();
    void upload(vector<Sequence> sequences);
    void search(Sequence sequence);
    void echo(QString msg);
    void closeConnection();
    ~Client();
private slots:
    void sendRequest();
    void stop();
    void connectionClosedByServer();
    void error();
    void getData();
private:
    void connectToServer();
    void search();
    void insert();
    void echo();
    QTcpSocket tcpSocket;
    quint16 nextBlockSize;
    quint8 requestType;

};

#endif // CLIENT_H
