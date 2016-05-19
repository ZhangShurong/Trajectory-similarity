#include "searchwin.h"
#include "ui_mainwindow.h"
SearchWin::SearchWin(Ui::MainWindow *ui,DataBase *db)
{
    this->ui = ui;
    this->db = db;
    numOfSeqs = 3;
    tracs = new QStringList();
    db = new DataBase(1);
    ui->searchMap->initJS();
    ui->searchMap->reload();
    initTable();
    initSig();
    //refreshTable();
}

void SearchWin::setTracs(QStringList *tracs)
{
    this->tracs = tracs;
}

void SearchWin::setDB(DataBase *db)
{
    this->db = db;
}


SearchWin::~SearchWin()
{

}

void SearchWin::initTable()
{
    ui->searchTable_common->horizontalHeader()->setStretchLastSection(true);
    ui->searchTable_common->verticalHeader()->hide();
    ui->searchTable_common->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->searchTable_common->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->searchTable_common->setColumnCount(4);
    ui->searchTable_common->setRowCount(20);
    ui->searchTable_common->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->searchTable_time->horizontalHeader()->setStretchLastSection(true);

    ui->searchTable_time->verticalHeader()->hide();
    ui->searchTable_time->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->searchTable_time->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->searchTable_time->setColumnCount(4);
    ui->searchTable_time->setRowCount(20);
    ui->searchTable_time->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void SearchWin::search(Sequence input)
{
    seqs.clear();
    Sequence sf;
    double dfDis;
    double maxDis = 0;
    string tb = "importtest";
    if (sf.pts->time == "")
        qDebug() << "No time";
    else {
        qDebug() << "Has time";
    }
    refreshTable();
    int c = 0;
    int t = 0;
    for (int i = 0;i < tracs->length();i++)
    {
        QTableWidgetItem *tItem = new QTableWidgetItem();
        db->getSequenceByID(tb,&sf,QString((*tracs)[i]).toStdString());
        if (sf.hasTime() && input.hasTime())
        {
            dfDis = computeDiscreteFrechet(&input,&sf);
            tItem->setData(Qt::DisplayRole,dfDis);
            ui->searchTable_time->setItem(t,2,tItem);
            t++;
        }
        else if (!sf.hasTime() && !input.hasTime()) {
            dfDis = computeDiscreteFrechet(&input,&sf);
            tItem->setData(Qt::DisplayRole,dfDis);
            ui->searchTable_common->setItem(c,2,tItem);
            c++;
        }
        if (dfDis >= maxDis)
        {
            maxDis = dfDis;
        }
        //qDebug() << dfDis;
    }
    if ( c != 0)
    {
        ui->searchTable_common->sortItems(2,Qt::AscendingOrder);
        for (int i = 0; i < c; i++)
        {
            if (i < numOfSeqs) {
                QString id = ui->searchTable_common->item(i, 0)->text();
                db->getSequenceByID(tb,&sf,id.toStdString());
                seqs.append(sf);
            }

            QTableWidgetItem *tItem = new QTableWidgetItem();
            double dis = ui->searchTable_common->item(i, 2)->text().toDouble();
            double percent = (maxDis - dis)/maxDis * 100;
            if (percent < 0)
            {
                percent = 0;
            }
            tItem->setData(Qt::DisplayRole,
                           QString::number(percent) + "%");
            ui->searchTable_common->setItem(i,3,tItem);
        }
    }
    else if (t != 0)
    {
        ui->searchTable_time->sortItems(2,Qt::AscendingOrder);
        for (int i = 0; i < t; i++)
        {
            if (i < numOfSeqs)
            {
                QString id = ui->searchTable_time->item(i, 0)->text();
                db->getSequenceByID(tb,&sf,id.toStdString());
                seqs.append(sf);
            }
            QTableWidgetItem *tItem = new QTableWidgetItem();
            double dis = ui->searchTable_time->item(i, 2)->text().toDouble();
            double percent = (maxDis - dis)/maxDis * 100;
            if (percent < 0)
            {
                percent = 0;
            }
            tItem->setData(Qt::DisplayRole,
                           QString::number(percent) + "%");
            ui->searchTable_time->setItem(i,3,tItem);
        }
    }
    ui->searchMap->initJS();
    ui->searchMap->setCentralPoint(getCenterPoint(seqs), 5);
    ui->searchMap->drawSequences(seqs);
    ui->searchMap->reload();
    qDebug() << "Search Over";
}

void SearchWin::initSig()
{
    connect(ui->searchStartBtn, SIGNAL(clicked()), this, SLOT(openFile()));
}


void SearchWin::refreshTable()
{
    //ui->mainTable->clearContents();
    ui->searchTable_common->clearContents();
    ui->searchTable_time->clearContents();
    QStringList header;
    header << tr("ID")
           << tr("PointNumber")
           << tr("Frechet Distance")
           << tr("Similarity");
    //ui->mainTable->setHorizontalHeaderLabels(header);
    ui->searchTable_common->setHorizontalHeaderLabels(header);
    ui->searchTable_time->setHorizontalHeaderLabels(header);
    if (tracs->length() == 0)
    {
        ui->searchTable_common->clearContents();
        ui->searchTable_time->clearContents();
    }
    else {
        int pos_c = 0;
        int pos_t = 0;
        for(int i = 0; i < tracs->length();i++)
        {
            QString temp = (*tracs)[i];
            if (db->hasTime(temp.toStdString(),"importtest"))
            {
                ui->searchTable_time->setItem(pos_t,0, new QTableWidgetItem(temp));
                ui->searchTable_time->setItem(pos_t,1,
                                   new QTableWidgetItem(QString::number(db->getPointNumByID(temp.toStdString()))));
                pos_t ++;
            }
            else
            {
                ui->searchTable_common->setItem(pos_c,0, new QTableWidgetItem(temp));
                ui->searchTable_common->setItem(pos_c,1,
                                   new QTableWidgetItem(QString::number(db->getPointNumByID(temp.toStdString()))));
                pos_c ++;
            }

        }
    }
}

void SearchWin::setNumOfSeqs(int num)
{
    numOfSeqs = num;
}


void SearchWin::openFile()
{
    QString file_name = QFileDialog::getOpenFileName(NULL,
            tr("Open File"),
            "",
            "CSV Files(*.csv)",
            0);
    if (!file_name.isNull())
        {
            qDebug()<<file_name;
        }
        else{
            return;
    }
    string fileName = file_name.toLocal8Bit().data();
    ifstream fin(fileName.c_str());
    Csv csv(fin);
    Sequence inputSe;
    getSquFromFile(&csv,&inputSe);
    qDebug() << inputSe.getNum();
    search(inputSe);
}

