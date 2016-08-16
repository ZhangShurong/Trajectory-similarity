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
    std::cout << "Socket " << this->socketDescriptor() <<" is closed and the connection will be cleared!\n";
    nextBlockSize = 0;
    db = new DataBase("Server");
    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
}

ClientSocket::~ClientSocket()
{
    db->closeConnection("");
    delete db;
}
void ClientSocket::readClient()
{
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
        QDataStream out(this);
        out << quint16(0xffff);
        return;
    }
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
    QDataStream out(this);
    out << quint16(0xffff);
}

void ClientSocket::echo(QString msg)
{
    std::cout << "Echo << \""<<msg.toStdString() << " \"" << std::endl;
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
    //out << quint16(0xffff);
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
    closeConnection();
}

void ClientSocket::searchInDB(Sequence sequence)
{
    QString msg = "Server got the sequence and the points number is "+ QString::number(sequence.getNum())
            + "\n";
    echo(msg);
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
    vector<Sequence> seq;
    loadIntoMemory(seq);
    QString msg = "In databasem, there is " + QString::number(seq.size()) + " Sequences\n";
    echo(msg);
    closeConnection();
}

void ClientSocket::searchResult()
{

}

void ClientSocket::loadIntoMemory(vector<Sequence> &seq)
{
    QStringList *tracIds = db->getAllTracID("Server");
    for(int i = 0; i < tracIds->size(); i++)
    {
        Sequence temp;
        db->getSequenceByID("Server", &temp, QString((*tracIds)[i]).toStdString());
        seq.push_back(temp);
    }
    delete tracIds;
}

void ClientSocket::download()
{
    QDataStream in(this);
    quint16 num;
    in >> num;
    if(num != -1)
    {
        //TODO Error report
        return;
    }
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    quint8 returnType;
    returnType = 'D';
    out<< quint16(0) << returnType;
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    write(block);
}

void ClientSocket::closeConnection()
{
    db->closeConnection("");
    QDataStream out(this);
    out << quint16(0xffff);
    //std::cout << this->socketDescriptor() <<" is Closed\n";
    //close();
}

