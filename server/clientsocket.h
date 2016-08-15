#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H
#include <QTcpSocket>
#include "sequence.h"
#include "database.h"
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

    void insertIntoDB(vector<Sequence> sequences);
    void insert();

    void searchInDB(Sequence sequence);
    void search();
    void searchResult();

    void download();
    quint16 nextBlockSize;
    DataBase *db;
};

#endif // CLIENTSOCKET_H
