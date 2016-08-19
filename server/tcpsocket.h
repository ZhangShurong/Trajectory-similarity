#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>
#include <QQueue>
#include <QFutureWatcher>
#include <QByteArray>

#include <QTime>
#include "sequence.h"
#include "database.h"


class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpSocket(qintptr socketDescriptor, QObject *parent = 0);
    ~TcpSocket();
    QByteArray handleData(QByteArray data,const QString & ip, qint16 port);//用来处理数据的函数

signals:
    //void readData(const int,const QString &,const quint16,const QByteArray &);
    void sockDisConnect(const int ,const QString &,const quint16, QThread *);//NOTE:断开连接的用户信息，此信号必须发出！线程管理类根据信号计数的
public slots:
    void sentData(const QByteArray & ,const int);//发送信号的槽
    void disConTcp(int i);

protected slots:
    void readData();//接收数据
    void startNext();//处理下一个
protected:
    QFutureWatcher<QByteArray> watcher;
    QQueue<QByteArray> datas;
private:
    qintptr socketID;
    QMetaObject::Connection dis;

    DataBase *db;
    quint32 nextBlockSize;
    void echo(QString msg);

    void insert();
    void insertIntoDB(vector<Sequence> sequences);
    void returnInsert(vector<Sequence> sequences);

    void search();
    void searchInDB(Sequence sequence, vector<Sequence> *seq);
    void returnSearch(vector<Result> resSet);


    void loadIntoMemory(vector<Sequence> &seq, int n);

    void refresh();
    void returnRefresh(vector<Sequence> sequences);

    void download();
    void returnDownload(vector<Sequence> sequences);

    void process(int p);

    //void returnRefresh(vector<Sequence> sequences);
};
void searchMul(Sequence sequence, vector<Sequence> *seq, int start, int end, double *resarr);

#endif // TCPSOCKET_H
