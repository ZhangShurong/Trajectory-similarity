#include "tcpsocket.h"
#include <QtConcurrent/QtConcurrent>
#include <QHostAddress>
#include <QDebug>



TcpSocket::TcpSocket(qintptr socketDescriptor, QObject *parent) : //构造函数在主线程执行，lambda在子线程
    QTcpSocket(parent),socketID(socketDescriptor)
{
    QString connName = "connName" + QString::number(socketDescriptor);
    db = new DataBase(connName, "Server");
    //db = new DataBase("Server");
    nextBlockSize = 0;
    this->setSocketDescriptor(socketDescriptor);
    connect(this,&TcpSocket::readyRead,this,&TcpSocket::readData);
    dis = connect(this,&TcpSocket::disconnected,
                  [&](){
        qDebug() << "disconnect" ;
        emit sockDisConnect(socketID,this->peerAddress().toString(),this->peerPort(),QThread::currentThread());//发送断开连接的用户信息
        this->deleteLater();
    });
    connect(&watcher,&QFutureWatcher<QByteArray>::finished,this,&TcpSocket::startNext);
    connect(&watcher,&QFutureWatcher<QByteArray>::canceled,this,&TcpSocket::startNext);
    qDebug() << "new connect" ;
}

TcpSocket::~TcpSocket()
{
    delete db;
}


void TcpSocket::sentData(const QByteArray &data, const int id)
{
    if(id == socketID)
    {
        write(data);
    }
}

void TcpSocket::disConTcp(int i)
{
    if (i == socketID)
    {
        this->disconnectFromHost();
    }
    else if (i == -1) //-1为全部断开
    {
        disconnect(dis); //先断开连接的信号槽，防止二次析构
        this->disconnectFromHost();
        this->deleteLater();
    }
}

void TcpSocket::readData()
{
    //    datas.append(this->readAll());
    //auto data  = handleData(this->readAll(),this->peerAddress().toString(),this->peerPort());
    //qDebug() << data;
    //this->write(data);

    std::cout << "Ready to read the client:\n";
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_5_4);
    if(nextBlockSize == 0)
    {
        //?????
        if(bytesAvailable() < sizeof(quint16))
            return;
        in >> nextBlockSize;
    }
    if(bytesAvailable() < nextBlockSize)
        return;
    quint8 requestType;
    QString msg;
    in >> requestType;
    std::cout << "Request type is " << requestType << std::endl;
    if(requestType == 'E')
    {
        in >> msg;
        std::cout << "Message is " <<msg.toStdString() << std::endl;
        echo(msg);
    }
    if(requestType == 'I')
    {
        insert();
    }
    if(requestType == 'S')
    {
        search();
    }
    if(requestType == 'R')
    {

    }
    nextBlockSize = 0;
    /*
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    quint8 returnType;
    returnType = 'E';
    QString msg = "New server";
    out<< quint16(0) << returnType
       << msg;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    write(block);
    qDebug() << msg;
    */
    //    if (!watcher.isRunning())//放到异步线程中处理。
    //    {
    //        watcher.setFuture(QtConcurrent::run(this,&TcpSocket::handleData,datas.dequeue(),this->peerAddress().toString(),this->peerPort()));
    //    }
}

QByteArray TcpSocket::handleData(QByteArray data, const QString &ip, qint16 port)
{
    QTime time;
    time.start();

    QElapsedTimer tm;
    tm.start();
    while(tm.elapsed() < 100)
    {}
    data = ip.toUtf8() + " " + QByteArray::number(port) + " " + data + " " + QTime::currentTime().toString().toUtf8();
    return data;
}

void TcpSocket::startNext()
{
    this->write(watcher.future().result());
    if (!datas.isEmpty())
    {
        watcher.setFuture(QtConcurrent::run(this,&TcpSocket::handleData,datas.dequeue(),this->peerAddress().toString(),this->peerPort()));
    }
}

void TcpSocket::echo(QString msg)
{
    std::cout << "Echo << \""<<msg.toStdString() << " \"" << std::endl;
    std::cout.flush();
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    quint8 returnType;
    returnType = 'E';
    out<< quint16(0) << returnType
       << msg;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    write(block);
}

void TcpSocket::insert()
{
    QDataStream in(this);
    quint16 num;
    in >> num;
    if(num <= 0)
    {
        //TODO Error report
        return;
    }
    vector<Sequence> sequences;
    for(uint i = 0; i < num; i++)
    {
        Sequence temp;
        in >> temp;
        sequences.push_back(temp);
    }
    if(sequences.size() == num)
    {
        insertIntoDB(sequences);
        echo(QString("Got sequences in insert"));
    }
}

void TcpSocket::search()
{
    QDataStream in(this);
    quint16 num;
    in >> num;
    if(num != 1)
    {
        //TODO Error report
        return;
    }
    Sequence temp;
    in >> temp;
    echo(QString("Got sequences in search\n"));
    searchInDB(temp);
    vector<Sequence> seq;
    loadIntoMemory(seq);
    QString msg = "In databasem, there is " + QString::number(seq.size()) + " Sequences\n";
    echo(msg);
}

void TcpSocket::searchInDB(Sequence sequence)
{
    QString msg = "Server got the sequence and the points number is "+ QString::number(sequence.getNum())
            + "\n";
    echo(msg);
}

void TcpSocket::insertIntoDB(vector<Sequence> sequences)
{
    if(sequences.size() <= 0)
    {
        return;
    }
    vector<string> stringList;
    db->db.transaction();
    for(uint i = 0; i < sequences.size(); i++)
    {
        stringList.push_back(db->insertData(sequences[i], "Server"));
    }
    db->db.commit();
    if(stringList.size() == sequences.size())
    {
        echo("Insert sucess and the first id is "+ QString::fromStdString(stringList.at(0)));
    }
}

void TcpSocket::loadIntoMemory(vector<Sequence> &seq)
{
    seq = db->getNSequences(10,"Server");
    std::cout<< seq.size();
    std::cout.flush();
    /*
    QStringList *tracIds = db->getAllTracID("Server");
    for(int i = 0; i < tracIds->size(); i++)
    {
        Sequence temp;
        db->getSequenceByID("Server", &temp, QString((*tracIds)[i]).toStdString());
        seq.push_back(temp);
    }
    delete tracIds;
    */
}
