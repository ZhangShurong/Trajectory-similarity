#include "clientsocket.h"
#include <iostream>
#include <QDataStream>

/* _______________________________________________________
 * |             |                  |           |        |
 * |size(quint16)|requestType(qint8)|num(qint16)|Sequence|
 * |_____________|__________________|___________|________|
 */

ClientSocket::ClientSocket(QObject *parent)
    :QTcpSocket(parent)
{
    nextBlockSize = 0;
    db = new DataBase("Server");
    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
}

ClientSocket::~ClientSocket()
{
    delete db;
}
void ClientSocket::readClient()
{
    std::cout << "In read client:\n";
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
    if(requestType == 'E')
    {
        in >> msg;
        std::cout << "Message is " <<msg.toStdString() << std::endl;
        echo(msg);
        QDataStream out(this);
        out << quint16(0xffff);
    }
    if(requestType == 'I')
    {
        insert();
    }
    if(requestType == 'S')
    {
        search();
    }
    QDataStream out(this);
    out << quint16(0xffff);
    close();
}

void ClientSocket::echo(QString msg)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out<< quint16(0)
       << msg;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    write(block);
}

void ClientSocket::insertIntoDB(vector<Sequence> sequences)
{
    if(sequences.size() <= 0)
    {
        return;
    }
    for(uint i = 0; i < sequences.size(); i++)
    {
        db->insertData(sequences[i], "Server");
    }
}

void ClientSocket::insert()
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

void ClientSocket::searchInDB(Sequence sequence)
{

}

void ClientSocket::search()
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
}

