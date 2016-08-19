#include "client.h"
#include <iostream>
#include <QProgressDialog>


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
    int c = 0;
    QProgressDialog importProgressDialog(tr("正在上传，请稍候..."),
                         tr("取消"),
                         0, sequences.size());
    importProgressDialog.setWindowModality(Qt::WindowModal);
    importProgressDialog.setWindowTitle(tr("Please Wait"));   //设置标题的显示时间
    importProgressDialog.show();

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
        if (importProgressDialog.wasCanceled()) {
            break;
            out.device()->seek(2);
            out << qint16(i);
        }
        c++;
        importProgressDialog.setValue(i);
    }
    out.device()->seek(0);
    out << quint32(block.size() - sizeof(quint32));
    this->write(block);
    importProgressDialog.setValue(sequences.size());
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
    std::cout << "In Refresh:\n";
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

void Client::download()
{
    std::cout << "In Download:\n";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    /* _______________________________________________________
     * |             |                  |           |        |
     * |size(quint16)|requestType(qint8)|num(qint16)|Sequence|
     * |_____________|__________________|___________|________|
     */
    qint16 num(-1);
    out << quint32(0) << qint8('D') << num ;
    out.device()->seek(0);
    out << quint32(block.size() - sizeof(quint32));
    this->write(block);
}
