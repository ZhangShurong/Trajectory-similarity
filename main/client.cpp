#include "client.h"
#include <iostream>


Client::Client()
{
    connect(&tcpSocket, SIGNAL(connected()), this, SLOT(sendRequest()));
    connect(&tcpSocket, SIGNAL(disconnected()), this, SLOT(connectionClosedByServer()));
    connect(&tcpSocket, SIGNAL(readyRead()), this, SLOT(getData()));
    connect(&tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
}

void Client::search(Sequence sequence)
{
    requestType = 'S';
    this->sequence = sequence;
    connectToServer();
}

void Client::echo(QString msg)
{
    requestType = 'E';
    this->msg = msg;
    connectToServer();
}

void Client::upload(vector<Sequence> sequences)
{
    requestType = 'I';
    this->sequences = sequences;
    connectToServer();
}

Client::~Client()
{

}

void Client::connectToServer()
{
    tcpSocket.connectToHost(QHostAddress::LocalHost, 10086);
    nextBlockSize = 0;
}

void Client::search()
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
    out << qint16(0) << qint8('S') << num ;
    out << sequence;
    out.device()->seek(0);
    out << qint16(block.size() - sizeof(quint16));
    tcpSocket.write(block);
    std::cout << "send over";
}

void Client::insert()
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
    out << qint16(0) << qint8('I') << num ;
    for(uint i = 0; i < sequences.size(); i++)
    {
        out << sequences[i];
    }
    out.device()->seek(0);
    out << qint16(block.size() - sizeof(quint16));
    tcpSocket.write(block);
    qDebug() << "Insert over";
}

void Client::echo()
{
    std::cout << "In echo:\n";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << qint16(0) << qint8('E') << msg;
    out.device()->seek(0);
    out << qint16(block.size() - sizeof(quint16));
    tcpSocket.write(block);
    std::cout << "echo over";
}

void Client::sendRequest()
{
    qDebug() << "In send request";
    if(requestType == 'I')
    {
        insert();
        return;
    }
    if(requestType == 'S')
    {
        search();
        return;
    }
    if(requestType == 'E')
    {
        echo();
        return;
    }

}

void Client::stop()
{
    tcpSocket.close();
}

void Client::connectionClosedByServer()
{
    if(nextBlockSize != 0xffff)
        std::cerr<<"Connection close by server\n";
    closeConnection();
}

void Client::error()
{
    std::cerr << "Error \n";
    tcpSocket.close();
}

void Client::getData()
{

    std::cout << "In getData";
    QDataStream in(&tcpSocket);
    in.setVersion(QDataStream::Qt_5_4);
    forever
    {
        if(nextBlockSize == 0)
        {
            if(tcpSocket.bytesAvailable() < sizeof(qint16))
                break;
            in >> nextBlockSize;
        }
        if(nextBlockSize == 0xffff)
        {
            closeConnection();
            break;
        }
        if(tcpSocket.bytesAvailable() < nextBlockSize)
        {
            break;
        }
        quint8 returnType;
        in >> returnType;
        if(returnType == 'E')
        {
            QString msg;
            in >> msg;
            QMessageBox::information(NULL, "提示", msg, QMessageBox::Yes, QMessageBox::Yes);
        }
        if(returnType == 'I')
        {

        }
        if(returnType == 'R')
        {

        }
        if(returnType == 'D')
        {

        }
        if(returnType == 'S')
        {

        }

        nextBlockSize = 0;
    }
}

void Client::closeConnection()
{
    tcpSocket.close();
}

