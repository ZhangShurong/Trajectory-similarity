#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H
#include <QTcpSocket>

class ClientSocket :public QTcpSocket
{
    Q_OBJECT
public:
    ClientSocket(QObject *parent = 0);
    ~ClientSocket();
private slots:
    void readClient();
private:
    void echo(QString msg);
    quint16 nextBlockSize;
};

#endif // CLIENTSOCKET_H
