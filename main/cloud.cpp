#include "cloud.h"
#include "core.h"
#include "ui_mainwindow.h"
#define ROW_NUM 100

Cloud::Cloud(Ui::MainWindow *ui, QWidget *parent)
:QMainWindow(parent)
{
    this->ui = ui;
    ipAdd = "127.0.0.1";
    portd = 10086;
    client = new Client();

    ui->progressBar->hide();    
    initTable();
    connect(ui->uploadBtn, SIGNAL(clicked()), this, SLOT(openfiles()));
    connect(ui->searchInServerBtn, SIGNAL(clicked(bool)), this, SLOT(openfile()));
    connect(ui->connectPushBtn, SIGNAL(clicked()), this, SLOT(connectPushBtnClicked()));


    connect(client, SIGNAL(connected()), this, SLOT(connectedMsg()));
    connect(client, SIGNAL(disconnected()), this, SLOT(connectionClosedByServer()));
    connect(client, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
}

Cloud::~Cloud()
{
    delete client;
}

void Cloud::openfiles()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open File"),
                            "", "CSV Files(*.csv)", 0);
    if (fileNames.isEmpty()) {
        return;
    }
    string fileName;
    vector<Sequence> sequences;
    for (int i = 0; i < fileNames.length(); i++)
        {
            fileName = fileNames[i].toLocal8Bit().data();
            ifstream fin2(fileName.c_str());
            Csv format(fin2);
            Sequence temp;
            try{
                getSquFromFile(&format,&temp);
            }
            catch(int i)
            {
                qDebug() << "Error Code is" << i;
                return;
            }
            sequences.push_back(temp);
        }
    client->upload(sequences);
}

void Cloud::openfile()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "", "CSV Files(*.csv)", 0);
    if(file_name.isEmpty())
    {
        return;
    }
    string fileName = file_name.toLocal8Bit().data();
    ifstream fin(fileName.c_str());
    Csv csv(fin);
    Sequence sequence;
    try
    {
        getSquFromFile(&csv, &sequence);
    }
    catch (int i)
    {
        qDebug() << "Error Code is" << i;
        return;
    }
    client->search(sequence);
}

void Cloud::refreshBtnClicked()
{

}

void Cloud::connectPushBtnClicked()
{
    if ("连接" == this->ui->connectPushBtn->text())
    {
        connectServer();
    }
    else {
        client->disconnectFromHost();
        if (client->state() == QAbstractSocket::UnconnectedState || client->waitForDisconnected(1000) )
        {
            ui->connectPushBtn->setText("连接");
            ui->serverInfo->setText("连接已断开");
        }
    }

}

void Cloud::connectedMsg()
{
    ui->connectPushBtn->setText("断开");
}

void Cloud::stop()
{
    client->close();
}

void Cloud::connectionClosedByServer()
{
    if(client->nextBlockSize != 0xffff)
        std::cerr<<"Connection close by server\n";
    client->disconnectFromHost();
    client->close();
}

void Cloud::error()
{
    std::cerr << "Error \n";
    client->close();
}

void Cloud::readData()
{
    std::cout << "In getData";
    QDataStream in(client);
    in.setVersion(QDataStream::Qt_5_4);
    forever
    {
        if(client->nextBlockSize == 0)
        {
            if(client->bytesAvailable() < sizeof(quint32))
                break;
            in >> client->nextBlockSize;
        }
        if(client->nextBlockSize == 0xffff)
        {
            client->close();
            break;
        }
        if(client->bytesAvailable() < client->nextBlockSize)
        {
            break;
        }
        quint8 returnType;
        in >> returnType;
        if(returnType == 'E')
        {
            QString msg;
            in >> msg;
            //QMessageBox::information(NULL, "提示", msg, QMessageBox::Yes, QMessageBox::Yes);
            std::cerr << msg.toStdString();
        }
        if(returnType == 'I')
        {
            vector<Sequence> res = getSequences();
            display(res);
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

        client->nextBlockSize = 0;
    }
}

void Cloud::connectServer()
{
    client->connectToServer(ipAdd, portd);
    if (client->waitForConnected(1000))
    {
        ui->serverInfo->setText("连接成功");
        ui->connectPushBtn->setText("断开");
         //QMessageBox::information(NULL, "提示", "连接成功", QMessageBox::Yes, QMessageBox::Yes);
    }
    else
    {
        ui->serverInfo->setText("连接已断开");
         QMessageBox::information(NULL, "提示", "连接失败", QMessageBox::Yes, QMessageBox::Yes);
    }
}

void Cloud::initTable()
{
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setRowCount(100);
    ui->tableWidget->clearContents();
    QStringList header;
    header << tr("轨迹ID")
           << tr("点数")
           << tr("相似系数（越小越相似）")
           << tr("备注");
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void Cloud::display(vector<Sequence> sequences)
{
    ui->tableWidget->clearContents();
    QStringList header;
    header << "轨迹ID"
           << "轨迹点数"
           << "Frechet Distance"
           << "相似度";
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if (sequences.size() == 0)
        ui->tableWidget->clear();
    else {
        if (sequences.size() > ROW_NUM) {
            ui->tableWidget->setRowCount(sequences.size() + 10);
        } else {
            ui->tableWidget->setRowCount(ROW_NUM + 10);
        }
        for(int i = 0; i < sequences.size(); i++) {
            ui->tableWidget->setItem(i,0, new QTableWidgetItem(QString::number(i));
            ui->tableWidget->setItem(i,1,
                                   new QTableWidgetItem(QString::number(sequences.at(i).getNum())));
        }
    }
}

vector<Sequence> Cloud::getSequences()
{
    QDataStream in(this->client);
    quint16 num;
    in >> num;
    if(num <= 0)
    {
        //TODO Error report
        ;
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

    }
    return sequences;
}

void Cloud::disconnectServer()
{
    client->disconnectFromHost();
    if (client->state() == QAbstractSocket::UnconnectedState || client->waitForDisconnected(1000) )
    {
        QMessageBox::information(NULL, "提示", "连接断开", QMessageBox::Yes, QMessageBox::Yes);
    }
}

