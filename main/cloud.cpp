#include "cloud.h"
#include "core.h"
#include "ui_mainwindow.h"
#define ROW_NUM 100

Cloud::Cloud(Ui::MainWindow *ui, QWidget *parent)
:QMainWindow(parent)
{
    this->ui = ui;
    //ipAdd = "121.42.202.133";
    ipAdd = "127.0.0.1";
    portd = 10086;
    client = new Client();
    connected = false;

    ui->progressBar->hide();    
    initTable();
    connect(ui->uploadBtn, SIGNAL(clicked()), this, SLOT(openfiles()));
    connect(ui->searchInServerBtn, SIGNAL(clicked(bool)), this, SLOT(openfile()));
    connect(ui->connectPushBtn, SIGNAL(clicked()), this, SLOT(connectPushBtnClicked()));
    connect(ui->refreshBtn, SIGNAL(clicked(bool)), this, SLOT(refreshBtnClicked()));
    connect(ui->dowmloadBtn, SIGNAL(clicked(bool)), this, SLOT(downloadPushBtnClicked()));

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
    if(!connected)
    {
        QMessageBox::information(NULL, "提示", "未连接服务器", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open File"),
                            "", "CSV Files(*.csv)", 0);
    if (fileNames.isEmpty()) {
        return;
    }
    string fileName;
    vector<Sequence> sequences;
    QProgressDialog importProgressDialog(tr("正在读取轨迹数据，请稍候..."),
                         tr("取消"),
                         0, fileNames.length());
    importProgressDialog.setWindowModality(Qt::WindowModal);
    importProgressDialog.setWindowTitle(tr("Please Wait"));   //设置标题的显示时间
    importProgressDialog.show();
    int c = 0;
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
            if (importProgressDialog.wasCanceled()) {
                break;
            }
            c++;
            importProgressDialog.setValue(i);
        }
    importProgressDialog.setValue(fileNames.length());
    client->upload(sequences);
    QMessageBox::information(NULL, "提示", "轨迹上传完成，请耐心等待服务器处理", QMessageBox::Yes, QMessageBox::Yes);
}

void Cloud::openfile()

{
    if(!connected)
    {
        QMessageBox::information(NULL, "提示", "未连接服务器", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
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
    if(!connected)
    {
        QMessageBox::information(NULL, "提示", "未连接服务器", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    client->refresh();
}

void Cloud::connectPushBtnClicked()
{

    if ("连接" == this->ui->connectPushBtn->text())
    {
        connectServer();
    }
    else {
        connected = false;
        QMessageBox::information(NULL, "提示", "连接已断开", QMessageBox::Yes, QMessageBox::Yes);
        client->disconnectFromHost();
        if (client->state() == QAbstractSocket::UnconnectedState || client->waitForDisconnected(1000) )
        {
            ui->connectPushBtn->setText("连接");
            ui->serverInfo->setText("连接已断开");
        }
    }

}

void Cloud::downloadPushBtnClicked()
{
    if(!connected)
    {
        QMessageBox::information(NULL, "提示", "未连接服务器", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    client->download();

}

void Cloud::connectedMsg()
{
    ui->connectPushBtn->setText("断开");
}

void Cloud::stop()
{
    client->close();
    connected = false;
    ui->serverInfo->setText("连接已断开");
    ui->connectPushBtn->setText("连接");
}

void Cloud::connectionClosedByServer()
{
    if(client->nextBlockSize != 0xffff)
    {
        if(connected)
            QMessageBox::warning(this,"Warning",tr("网络连接异常关闭"),QMessageBox::Yes,QMessageBox::Yes);
    }
    connected = false;
    ui->serverInfo->setText("连接已断开");
    ui->connectPushBtn->setText("连接");
    client->disconnectFromHost();
    client->close();
}

void Cloud::error()
{
    QMessageBox::warning(this,"Warning",tr("网络连接出现问题"),QMessageBox::Yes,QMessageBox::Yes);
    client->close();
    connected = false;
    ui->serverInfo->setText("连接已断开");
    ui->connectPushBtn->setText("连接");
}

void Cloud::readData()
{    
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
            QMessageBox::information(NULL, "提示", msg, QMessageBox::Yes, QMessageBox::Yes);
        }
        if(returnType == 'I')
        {
            vector<Sequence> res = getSequences();
            display(res);
        }
        if(returnType == 'R')
        {
            vector<Sequence> res = getSequences();
            display(res);
        }
        if(returnType == 'D')
        {
            vector<Sequence> res = getSequences();
            display(res);
            download(res);
        }
        if(returnType == 'S')
        {
            vector<Result> resSet = getResSet();
            displayRes(resSet);
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
        connected = true;
        QMessageBox::information(NULL, "提示", "连接成功", QMessageBox::Yes, QMessageBox::Yes);
    }
    else
    {
        ui->serverInfo->setText("连接已断开");
        connected = false;
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
        for(size_t i = 0; i < sequences.size(); i++) {
            if(sequences.at(i).getID() == "Input")
                ui->tableWidget->setItem(i,0, new QTableWidgetItem(QString::number(i)));
            else
                ui->tableWidget->setItem(i,0, new QTableWidgetItem(sequences.at(i).getID()));
            ui->tableWidget->setItem(i,1,
                                   new QTableWidgetItem(QString::number(sequences.at(i).getNum())));
        }
    }
}

void Cloud::displayRes(vector<Result> resSet)
{
    ui->tableWidget->clearContents();
    QStringList header;
    header << "轨迹ID"
           << "轨迹点数"
           << "Frechet Distance"
           << "demo";
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if (resSet.size() == 0)
        ui->tableWidget->clear();
    else {
        if (resSet.size() > ROW_NUM) {
            ui->tableWidget->setRowCount(resSet.size() + 10);
        } else {
            ui->tableWidget->setRowCount(ROW_NUM + 10);
        }
        for(size_t i = 0; i < resSet.size(); i++) {
                ui->tableWidget->setItem(i,0, new QTableWidgetItem(resSet.at(i).id));
                ui->tableWidget->setItem(i,1,
                                   new QTableWidgetItem(QString::number(resSet.at(i).ptNum)));
                ui->tableWidget->setItem(i,2,new QTableWidgetItem(QString::number(resSet.at(i).res)));
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

vector<Result> Cloud::getResSet()
{
    /* ____________________________________________________________________________________
     * |             |                  |           |                                      |
     * |size(quint16)|requestType(qint8)|num(qint16)|QString,quint16,quint8,QString,QString|
     * |_____________|__________________|___________|______________________________________|
     */

    QDataStream in(this->client);
    quint16 num;
    in >> num;
    if(num <= 0)
    {
        //TODO Error report
    }
    vector<Result> resSet;
    for(uint i = 0; i < num; i++)
    {
        Result temp;
        quint16 ptNum;
        quint8 time;
        in >> temp.id
                >> ptNum
                >> time;
        temp.time = time;
        temp.ptNum = ptNum;
        QString res;
        in >> res;
        temp.res = res.toDouble();
        in >> temp.demo;
        resSet.push_back(temp);
    }
    if(resSet.size() == num)
    {

    }
    return resSet;
}

void Cloud::download(vector<Sequence> seqs)
{
    QString dirname = QFileDialog::getExistingDirectory(this, "选择文件夹","");
    if(dirname.isEmpty()) {
        return;
    } else {
        if (seqs.size() == 0) {
            return;
        } else {
            QProgressDialog downloadProgressDialog(tr("正在下载数据，请稍候..."),
                                 tr("取消"),
                                 0, seqs.size());
            downloadProgressDialog.setWindowModality(Qt::WindowModal);
            downloadProgressDialog.setWindowTitle(tr("Please Wait"));   //设置标题的显示时间
            downloadProgressDialog.show();

            for (uint i = 0; i< seqs.size(); i++) {
                QString filename = dirname + "/"+ QString::number(i) + ".csv";
                QFile file(filename);
                if (!file.open(QIODevice::WriteOnly)) {
                    qDebug() << "Failed!";
                    return;
                }
                QTextStream out(&file);
                for (int j = 0; j < seqs[i].getNum(); j++) {
                    out << seqs[i].pts[j].longitude
                        << ","
                        << seqs[i].pts[j].latitude
                        << "\n";
                    out.flush();
                }
                file.close();
                if (downloadProgressDialog.wasCanceled()) {
                    break;
                }
                downloadProgressDialog.setValue(i);
            }
            downloadProgressDialog.setValue(seqs.size());
            QMessageBox::information(NULL, "提示", "轨迹传输完成", QMessageBox::Yes, QMessageBox::Yes);
        }
    }
}

void Cloud::disconnectServer()
{
    client->disconnectFromHost();
    connected = false;
    if (client->state() == QAbstractSocket::UnconnectedState || client->waitForDisconnected(1000) )
    {
        QMessageBox::information(NULL, "提示", "连接断开", QMessageBox::Yes, QMessageBox::Yes);
    }
}

