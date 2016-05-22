#include "searchwin.h"
#include "ui_mainwindow.h"
SearchWin::SearchWin(Ui::MainWindow *ui,DataBase *db)
{
    this->ui = ui;
    this->db = db;
    time = false;
    numOfSeqs = 3;
    tracs = new QStringList();
    db = new DataBase(1);
    ui->searchMap->initJS();
    ui->searchMap->reload();
    initTable(ui->searchTable_common);
    initTable(ui->searchTable_common_part);
    initTable(ui->searchTable_common_point);
    initTable(ui->searchTable_time);
    initTable(ui->searchTable_time_part);
    initTable(ui->searchTable_time_point);
    initSeqPartTable(ui->searchTable_common_part);
    initSeqPartTable(ui->searchTable_time_part);
    ui->searchStackedWidget->setCurrentIndex(0);
    ui->searchStackedWidget_time->setCurrentIndex(0);
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

void SearchWin::initTable(QTableWidget *table)
{
        table->horizontalHeader()->setStretchLastSection(true);
        table->verticalHeader()->hide();
        table->setContextMenuPolicy(Qt::ActionsContextMenu);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setColumnCount(4);
        table->setRowCount(20);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void SearchWin::initSeqPartTable(QTableWidget *table)
{
    table->setColumnCount(4);
    table->setRowCount(20);
    table->clearContents();
    QStringList header;
    header << tr("ID")
           << tr("PointNumber")
           << tr("StartPoint")
           << tr("EndPoint");
    table->setHorizontalHeaderLabels(header);
}

void SearchWin::showPartofSeq()
{
    qDebug() << "shoe Part Of Sequence\n";
}

void SearchWin::search(Sequence input)
{
    seqs.clear();
    Sequence sf;
    double dfDis;
    double maxDis = 0;
    string tb = "importtest";
    if (sf.pts->time == "")
    {
        qDebug() << "No time";
        time = false;
    }

    else {
        time = true;
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
    connect(ui->searchPartCBox, SIGNAL(clicked()), this, SLOT(rankPartOfSeq()));
    connect(ui->searchSequenceCBox, SIGNAL(clicked()), this, SLOT(rankSeqChecked()));
    connect(ui->searchPointCBox, SIGNAL(clicked()), this, SLOT(rankSeqPointChecked()));
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
    ui->searchPathEdit->setText(file_name);
    search(inputSe);
}

void SearchWin::rankPartOfSeq()
{
    bool flag = (ui->searchPartCBox->checkState() == Qt::Checked);
    ui->searchPointCBox->setDisabled(flag);
    ui->searchSequenceCBox->setDisabled(flag);
    if (flag)
    {
        ui->searchStackedWidget->setCurrentIndex(2);
        ui->searchStackedWidget_time->setCurrentIndex(2);
        showPartofSeq();
    }
    else
    {
        ui->searchStackedWidget->setCurrentIndex(0);
        ui->searchStackedWidget_time->setCurrentIndex(0);
    }
}

void SearchWin::rankSeqChecked()
{
    bool flag = (ui->searchSequenceCBox->checkState() == Qt::Checked);
    ui->searchPointCBox->setDisabled(flag);
    ui->searchPartCBox->setDisabled(flag);
    if (flag)
    {
        ui->searchStackedWidget->setCurrentIndex(0);
        ui->searchStackedWidget_time->setCurrentIndex(0);
        showPartofSeq();
    }
    else
    {
        ui->searchStackedWidget->setCurrentIndex(0);
        ui->searchStackedWidget_time->setCurrentIndex(0);
    }
}

void SearchWin::rankSeqPointChecked()
{
    bool flag = (ui->searchPointCBox->checkState() == Qt::Checked);
    ui->searchSequenceCBox->setDisabled(flag);
    ui->searchPartCBox->setDisabled(flag);
    if (flag)
    {

        ui->searchStackedWidget->setCurrentIndex(1);
        ui->searchStackedWidget_time->setCurrentIndex(1);
        showPartofSeq();
    }
    else
    {
        ui->searchStackedWidget->setCurrentIndex(0);
        ui->searchStackedWidget_time->setCurrentIndex(0);
    }
}

