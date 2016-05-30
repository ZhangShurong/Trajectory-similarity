#include "searchwin.h"
#include "ui_mainwindow.h"
SearchWin::SearchWin(Ui::MainWindow *ui,DataBase *db)
{
    this->ui = ui;
    this->db = db;
    input = new Sequence();
    time = false;
    distinct = true;
    numOfSeqs = 3;
    tracs = new QStringList();
    rowcount = 0;
    partRowcount = 0;
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
    initPointTable(ui->searchTable_common_point);
    initPointTable(ui->searchTable_time_point);

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
    delete input;
}

void SearchWin::initTable(QTableWidget *table)
{
        //table->horizontalHeader()->setStretchLastSection(true);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->verticalHeader()->hide();
        table->setContextMenuPolicy(Qt::ActionsContextMenu);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setColumnCount(4);
        table->setRowCount(ROW_NUM);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void SearchWin::initSeqPartTable(QTableWidget *table)
{
    table->setColumnCount(4);
    table->setRowCount(ROW_NUM);
    table->clearContents();
    QStringList header;
    header << tr("ID")
           << tr("PointNumber")
           << tr("StartPoint")
           << tr("EndPoint");
    table->setHorizontalHeaderLabels(header);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void SearchWin::initPointTable(QTableWidget *table)
{    
    table->setColumnCount(5);
    table->setRowCount(ROW_NUM);
    table->clearContents();
    QStringList header;
    header << "轨迹ID"
           << "点在轨迹中位置"
           << "地图中编号"
           << "DIS"
           << "点在输入轨迹中位置";

    table->setHorizontalHeaderLabels(header);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void SearchWin::showPartofSeq()
{
    //qDebug() << "show Part Of Sequence\n";
}

void SearchWin::drawPoints()
{

    QString sid = "";
    int index2 = 0;
    int index1 = 0;
    double dis = 0;
    int num = 0;
    for (int j = 0; j < rowcount; j ++)
    {


        if (!time)
        {
            sid =  ui->searchTable_common_point->item(j,0)->text();
            index2 =  ui->searchTable_common_point->item(j,1)->text().toInt();
            index1 =  ui->searchTable_common_point->item(j,4)->text().toInt();
            dis = ui->searchTable_common_point->item(j,3)->text().toDouble();
            if (dis !=0 )
                num = ui->searchTable_common_point->item(j,2)->text().toInt();
        }
        else
        {
            sid =  ui->searchTable_time_point->item(j,0)->text();
            index2 = ui->searchTable_time_point->item(j,1)->text().toInt();
            index1 =  ui->searchTable_time_point->item(j,4)->text().toInt();
            dis = ui->searchTable_time_point->item(j,3)->text().toDouble();
            if (dis !=0 )
                num = ui->searchTable_time_point->item(j,2)->text().toInt();
        }

        if (dis > 1)
        {
            break;
        }
        if (dis == 0){
            ui->searchMap->drawPoint(&((id_seq_map[sid]).pts[index2]),"",seq_index[sid],true);
        }
        else
        {
            ui->searchMap->drawPoint(&((id_seq_map[sid]).pts[index2]),"",seq_index[sid],num);
            ui->searchMap->drawPoint(&(input->pts[index1]),"",seq_index[sid],num);
        }
       // ui->searchMap->drawPoint(&((seqs[seq_index[sid]])[index1]),"",seq_index[sid],j);

    }
    seq_index.clear();
}

void SearchWin::search()
{
    fillTable(*input);//填充三个表格
    if (!time)
    {
        sortPointTable(ui->searchTable_common_point);
    }
    else
    {
        sortPointTable(ui->searchTable_time_point);
    }

    ui->searchMap->initJS();
    ui->searchMap->showPoints(true);
    ui->searchMap->showTimes(true);
    ui->searchMap->setCentralPoint(getCenterPoint(seqs), 5);
    ui->searchMap->setFilter(time);
    ui->searchMap->drawSequences(seqs, coincide);
    drawPoints();
    ui->searchMap->reload();

    qDebug() << "Search Over";

}

void SearchWin::sortPointTable(QTableWidget *table)
{
    table->sortItems(3, Qt::AscendingOrder);
    int length = rowcount;
    int j = 0;
    for (int i =0; i < length; i++)
    {
        if (table->item(i,3)->text().toDouble() == 0)
        {
            table->setItem(i,2, new QTableWidgetItem(""));
            continue;
        }

        table->setItem(i,2, new QTableWidgetItem(QString::number(j)));
        j ++;
        seq_index.insert(table->item(i,0)->text(), table->item(i,2)->text().toInt());
    }

}

void SearchWin::initSig()
{
    connect(ui->searchStartBtn, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->searchPartCBox, SIGNAL(clicked()), this, SLOT(rankPartOfSeq()));
    connect(ui->searchSequenceCBox, SIGNAL(clicked()), this, SLOT(rankSeqChecked()));
    connect(ui->searchPointCBox, SIGNAL(clicked()), this, SLOT(rankSeqPointChecked()));
    connect(ui->searchWinBtn, SIGNAL(clicked()), this, SLOT(startSearch()));
}

void SearchWin::fillPointTable(QTableWidget *table, QVector<PointCompare> pointsV, Sequence *se)
{
   // qDebug() << "rowcount = "
     //        << QString::number(rowcount);
    if (distinct)
    {
        if (coincide.contains(se->getID()))
        {
            return;
        }
    }
    int start = rowcount;
    int end = start + pointsV.length();
    if (end > rowcount)
    {
        table->setRowCount(end + 10);
    }
    else if (rowcount == 0)
    {
        table->setRowCount(ROW_NUM);
    }
    for (int x = start; x < end; x++)
    {
        table->setItem(x,0, new QTableWidgetItem(se->getID()));
        table->setItem(x,1, new QTableWidgetItem(
                                                QString::number(pointsV[x - start].index2)));

        table->setItem(x,3, new QTableWidgetItem(
                                                QString::number(pointsV[x - start].distance)));
        table->setItem(x,4, new QTableWidgetItem(
                                                QString::number(pointsV[x - start].index1)));
    }
    rowcount = end;
}

void SearchWin::fillPartTable(QTableWidget *table, QVector<QVector<int> > partInfo,
                              int beginMin1,int beginMin2,Sequence *se)
{
    if (distinct)
    {
        if (coincide.contains(se->getID()))
        {
            return;
        }
    }

    QVector<int>pv=partInfo[0];
    QVector<int>qv=partInfo[1];
    qDebug() << "size is "<< pv.size();
    for(int i=0; i<pv.size() - 1; i=i+2){
        int begin1=pv[i];
        int end1=pv[i+1];
        int begin2=qv[i];
        int end2=qv[i+1];
        table->setItem(partRowcount, 0, new QTableWidgetItem(se->getID()));
        table->setItem(partRowcount, 1, new QTableWidgetItem(QString::number(begin1+beginMin1)));
        table->setItem(partRowcount, 2, new QTableWidgetItem(QString::number(end1+beginMin1)));
        ui->searchMap->highLightPart(input, begin1+beginMin1, end1+beginMin1, 3, 10);
        ui->searchMap->highLightPart(se, begin2+beginMin2, end2+beginMin2, 3, 10);
        partRowcount ++;
    }
}

void SearchWin::fillTable(Sequence inSeq)
{
    string tb = "importtest";
    QVector<PointCompare> pVec;
    double dfDis;
    double maxDis = 0;
    int c = 0;
    int t = 0;

    seqs.append(inSeq);
    id_seq_map.insert(inSeq.getID(), inSeq);

    for (int i = 0;i < tracs->length();i++)
    {
        Sequence sf;
        QTableWidgetItem *tItem = new QTableWidgetItem();
        db->getSequenceByID(tb,&sf,QString((*tracs)[i]).toStdString());

        seqs.append(sf);
        id_seq_map.insert(sf.getID(), sf);

        dfDis = computeDiscreteFrechet(&inSeq,&sf);
        if (dfDis == 0)
        {
            coincide << sf.getID();
        }

        tItem->setData(Qt::DisplayRole,dfDis);
        pVec = getNearestPoint(&inSeq, &sf);

        if (sf.hasTime() && inSeq.hasTime())
        {
            ui->searchTable_time->setItem(t,2,tItem);
            fillPointTable(ui->searchTable_time_point, pVec, &sf);
            t++;
        }
        else if (!sf.hasTime() && !inSeq.hasTime())
        {
            ui->searchTable_common->setItem(c,2,tItem);
            fillPointTable(ui->searchTable_common_point, pVec, &sf);
            c++;
        }
        if (dfDis >= maxDis)
        {
            maxDis = dfDis;
        }
    }


    if ( c != 0)
    {
        time = false;
        ui->searchTable_common->sortItems(2,Qt::AscendingOrder);
        for (int i = 0; i < c; i++)
        {
            Sequence sf;
            if (i < numOfSeqs) {
                QString id = ui->searchTable_common->item(i, 0)->text();
                db->getSequenceByID(tb,&sf,id.toStdString());             
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
        time = true;
        ui->searchTable_time->sortItems(2,Qt::AscendingOrder);
        for (int i = 0; i < t; i++)
        {
            Sequence sf;
            if (i < numOfSeqs)
            {
                QString id = ui->searchTable_time->item(i, 0)->text();
                db->getSequenceByID(tb,&sf,id.toStdString());
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
}

void SearchWin::calSecPart()
{
    if (!input)
    {
        return;
    }
    else if (input->getNum() == 0)
    {
        return;
    }
    ui->searchMap->initJS();
    ui->searchMap->showPoints(true);
    ui->searchMap->showTimes(true);
    ui->searchMap->setCentralPoint(getCenterPoint(seqs), 5);
    ui->searchMap->drawSequences(seqs, coincide);

    QVector<QVector<int> >qc;
    int beginMin1=0;
    int beginMin2=0;
    string tb = "importtest";
    QVector<PointCompare> pVec;
    double dfDis;
    Sequence sf;

    for (int i = 0;i < tracs->length();i++)
    {
        db->getSequenceByID(tb,&sf,QString((*tracs)[i]).toStdString());
        int beginMin1,beginMin2;
        qc = getSimplify(input,&sf,beginMin1,beginMin2);

        if (sf.hasTime() && input->hasTime())
        {
            fillPartTable(ui->searchTable_time_part,
                      qc, beginMin1, beginMin2, &sf);
        }
        else if (!sf.hasTime() && !input->hasTime())
        {
            fillPartTable(ui->searchTable_common_part,
                      qc, beginMin1, beginMin2, &sf);
        }
    }
    ui->searchMap->reload();
    coincide.clear();
    seqs.clear();
    id_seq_map.clear();
    rowcount = 0;
    partRowcount = 0;
}

void SearchWin::refreshTable()
{
    initSeqPartTable(ui->searchTable_common_part);
    initSeqPartTable(ui->searchTable_time_part);
    initPointTable(ui->searchTable_common_point);
    initPointTable(ui->searchTable_time_point);
    ui->searchTable_common->clearContents();
    ui->searchTable_time->clearContents();
    QStringList header;
    header << tr("ID")
           << tr("PointNumber")
           << tr("Frechet Distance")
           << tr("Similarity");
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
        if (tracs->length() > ROW_NUM)
        {
            ui->searchTable_common->setRowCount(tracs->length() + 10);
            ui->searchTable_time->setRowCount(tracs->length() + 10);
        }
        else
        {
            ui->searchTable_common->setRowCount(ROW_NUM);
            ui->searchTable_time->setRowCount(ROW_NUM);
        }
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

void SearchWin::init()
{
    coincide.clear();
    seqs.clear();
    id_seq_map.clear();
    rowcount = 0;
    partRowcount = 0;

}


void SearchWin::openFile()
{

    if (input->getNum() != 0)
    {
        delete input;
        input = new Sequence();
    }
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
    getSquFromFile(&csv,input);
    ui->searchPathEdit->setText(file_name);
}

void SearchWin::rankPartOfSeq()
{
    calSecPart();
    bool flag = (ui->searchPartCBox->isChecked());
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
    bool flag = (ui->searchSequenceCBox->isChecked());
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
    bool flag = (ui->searchPointCBox->isChecked());
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

void SearchWin::startSearch()
{
    this->ui->searchMap->initJS();
    this->ui->searchMap->setDefaultCentralPt();
    this->ui->searchMap->reload();
    refreshTable();
    init();

    if (input->getNum() == 0)
    {
        return;
    }
    search();
}

