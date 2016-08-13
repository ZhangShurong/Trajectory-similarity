#include "clientsocket.h"
#include <iostream>
#include <QDataStream>
#include "sequence.h"

ClientSocket::ClientSocket(QObject *parent)
    :QTcpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
    nextBlockSize = 0;
}

ClientSocket::~ClientSocket()
{

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
    quint16 num;
    /* _______________________________________________________
     * |             |                  |           |        |
     * |size(quint16)|requestType(qint8)|num(qint16)|Sequence|
     * |_____________|__________________|___________|________|
     */
    if(requestType == 'I')
    {
        in >> num;
        if(num > 0)
        {
            Sequence *sequences = new Sequence[num];
            for(int i = 0; i < num; i++)
            {
                in >> sequences[i];
            }
            QString msg = "Got sequences and the num is "+ QString::number(num);
            echo(msg);
        }
        else
        {
            //todo error
        }
        echo("Insert\n");

        QDataStream out(this);
        out << quint16(0xffff);
    }
    else if(requestType == 'S')
    {
        in >> num;
        if(num != 1)
        {
            //todo error
        }
        else
        {
            Sequence sequence;
            in >> sequence;
            std::cout << sequence.getNum();
        }
        echo("Search\n");
        QDataStream out(this);
        out << quint16(0xffff);
    }
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

