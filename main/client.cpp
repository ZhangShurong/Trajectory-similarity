#include "client.h"
#include <iostream>


Client::Client()
{

}



Client::~Client()
{

}



void Client::connectToServer(QString ipaddr, uint port)
{
    if(ipaddr.isEmpty())
    {
        this->connectToHost(QHostAddress::LocalHost, 10086);
    }
    else {
        this->connectToHost(ipaddr, port);
    }
    nextBlockSize = 0;
}

void Client::search(Sequence sequence)
{
    std::cout << "In search:\n";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    /* _______________________________________________________
     * |             |                  |           |        |
     * |size(quint16)|requestType(qint8)|num(qint16)|Sequence|
     * |_____________|__________________|___________|________|
     */
    qint16 num(1);
    out << quint32(0) << qint8('S') << num ;
    out << sequence;
    out.device()->seek(0);
    out << quint32(block.size() - sizeof(quint32));
    this->write(block);
    std::cout << "send over and blocksize is " << quint32(block.size() - sizeof(quint32));
    std::cout.flush();
}

void Client::upload(vector<Sequence> sequences)
{
    std::cout << "In insert:\n";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    /* _______________________________________________________
     * |             |                  |           |        |
     * |size(quint16)|requestType(qint8)|num(qint16)|Sequence|
     * |_____________|__________________|___________|________|
     */
    qint16 num(sequences.size());
    out << quint32(0) << qint8('I') << num ;
    for(uint i = 0; i < sequences.size(); i++)
    {
        out << sequences[i];
    }
    out.device()->seek(0);
    out << quint32(block.size() - sizeof(quint32));
    this->write(block);
    qDebug() << "Insert over";
}

void Client::echo(QString msg)
{
    std::cout << "In echo:\n";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << quint32(0) << qint8('E') << msg;
    out.device()->seek(0);
    out << quint32(block.size() - sizeof(quint32));
    this->write(block);
    std::cout << "echo over";
}

void Client::refresh()
{
    std::cout << "In insert:\n";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    /* _______________________________________________________
     * |             |                  |           |        |
     * |size(quint16)|requestType(qint8)|num(qint16)|Sequence|
     * |_____________|__________________|___________|________|
     */
    qint16 num(-1);
    out << quint32(0) << qint8('R') << num ;
    out.device()->seek(0);
    out << quint32(block.size() - sizeof(quint32));
    this->write(block);
}
