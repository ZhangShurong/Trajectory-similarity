#include "client.h"
#include <iostream>
#include <QHostAddress>
#include <QObject>
#include <QDataStream>

Client::Client()
{
    connect(&tcpSocket, SIGNAL(connected()), this, SLOT(sendRequest()));
    connect(&tcpSocket, SIGNAL(disconnected()), this, SLOT(connectionClosedByServer()));
    connect(&tcpSocket, SIGNAL(readyRead()), this, SLOT(getData()));
    connect(&tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
}

Client::~Client()
{

}

void Client::connectToServer()
{
    tcpSocket.connectToHost(QHostAddress::LocalHost, 10086);
    nextBlockSize = 0;
}

void Client::sendRequest()
{
    std::cout << "In send";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << qint16(0) << qint8('T') << QString("Hello World");
    out.device()->seek(0);
    out << qint16(block.size() - sizeof(quint16));
    tcpSocket.write(block);
    std::cout << "send over";
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
        QString msg;
        in >> msg;
        std::cout << "The message is "
                  << msg.toStdString()
                  << std::endl;
        nextBlockSize = 0;
    }
}

void Client::closeConnection()
{
    tcpSocket.close();
}

