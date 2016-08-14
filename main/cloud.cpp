#include "cloud.h"
#include "core.h"
#include "ui_mainwindow.h"

Cloud::Cloud(Ui::MainWindow *ui, QWidget *parent)
:QMainWindow(parent)
{
    this->ui = ui;
    client = new Client();
    ui->progressBar->hide();
    ui->serverInfo->hide();
    initTable();
    connect(ui->uploadBtn, SIGNAL(clicked()), this, SLOT(openfiles()));
    connect(ui->searchInServerBtn, SIGNAL(clicked(bool)), this, SLOT(openfile()));
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
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "", "CSV Files(*.csv)", 0);
    if(filename.isEmpty())
    {
        return;
    }
    ifstream fin(filename.toStdString().c_str());
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

void Cloud::search(Sequence sequence)
{

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

