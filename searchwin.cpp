#include "searchwin.h"
#include "ui_mainwindow.h"
#include <QProgressDialog>
SearchWin::SearchWin(Ui::MainWindow *ui,DataBase *db)
{
    this->ui = ui;
    this->db = db;

    seqFlag = false;
    partFlag = false;
    pointFlag  = false;

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
    header << tr("轨迹段所在轨迹")
           << tr("起点-终点")
           << tr("对应起点-终点")
           << tr("相似系数（越小越相似）");
    table->setHorizontalHeaderLabels(header);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void SearchWin::initPointTable(QTableWidget *table)
{
    table->setColumnCount(4);
    table->setRowCount(ROW_NUM);
    table->clearContents();
    QStringList header;
    header << "轨迹点所在轨迹"
           << "点在轨迹中位置"
           << "点在输入轨迹中位置"
           << "DIS";

    table->setHorizontalHeaderLabels(header);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void SearchWin::showPartofSeq()
{
    //qDebug() << "show Part Of Sequence\n";
}


void SearchWin::searchSeq()
{
    double dfDis;
    double maxDis = 0;
    int c = 0;
    int t = 0;

    QProgressDialog progress(tr("正在计算轨迹数据，请稍候..."),
                             tr("取消"),
                             0, tracs->length());
    progress.setWindowModality(Qt::WindowModal);
    //progress.show();

    for (int i = 0; i < tracs->length(); i++)
    {
        Sequence sf;
        sf = id_seq_map[tracs->at(i)];
        QTableWidgetItem *tItem = new QTableWidgetItem();

        dfDis = computeDiscreteFrechet(input,&sf);

        if (dfDis == 0)
        {
            coincide << sf.getID();
        }
        tItem->setData(Qt::DisplayRole,dfDis);

        if (sf.hasTime() && input->hasTime())
        {
            ui->searchTable_time->setItem(t,2,tItem);
            t++;
        }
        else if (!sf.hasTime() && !input->hasTime())
        {
            ui->searchTable_common->setItem(c,2,tItem);
            c++;
        }
        if (dfDis >= maxDis)
        {
            maxDis = dfDis;
        }
        progress.setValue(i);
        if (progress.wasCanceled()) {
            break;
        }
    }
    progress.setValue(tracs->length());


    if ( c != 0)
    {
        time = false;
        ui->searchTable_common->sortItems(2,Qt::AscendingOrder);
        for (int i = 0; i < c; i++)
        {
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

void SearchWin::sortPointTable()
{
    ui->searchTable_common_point->sortItems(3, Qt::AscendingOrder);
    ui->searchTable_time_point->sortItems(3, Qt::AscendingOrder);
}

void SearchWin::sortPartTable()
{
    ui->searchTable_common_part->sortItems(3, Qt::AscendingOrder);
    ui->searchTable_time_part->sortItems(3, Qt::AscendingOrder);
}

void SearchWin::sortSeqTable()
{
    ui->searchTable_common->sortItems(2, Qt::AscendingOrder);
    ui->searchTable_time->sortItems(2, Qt::AscendingOrder);
}

void SearchWin::initSig()
{
    connect(ui->searchStartBtn, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->calSeqPartBtn, SIGNAL(clicked()), this, SLOT(rankPartOfSeq()));
    connect(ui->calSeqBtn, SIGNAL(clicked()), this, SLOT(rankSeqClicked()));
    connect(ui->calSeqPointBtn, SIGNAL(clicked()), this, SLOT(rankSeqPointClicked()));
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
        table->setItem(x,2, new QTableWidgetItem(
                           QString::number(pointsV[x - start].index1)));
    }
    rowcount = end;
}

void SearchWin::fillPartTable(QTableWidget *table, QVector<QVector<int> > partInfo,Sequence *se)
{
    if (distinct)
    {
        if (coincide.contains(se->getID()))
        {
            return;
        }
    }
    int end = partRowcount + partInfo[0].length();
    if (end > partRowcount)
    {
        table->setRowCount(end + 10);
    }
    else if (partRowcount == 0)
    {
        table->setRowCount(ROW_NUM);
    }
    /*
     *    header << tr("轨迹段所在轨迹")
           << tr("起点-终点")
           << tr("对应起点-终点")
           << tr("相似度");
     */
    QVector<int>pv=partInfo[0];
    QVector<int>qv=partInfo[1];
    QString str;
    initP_Q(input, se);
    initMemSpace(input, se);
    for(int i=0; i<pv.size() - 1; i=i+2) {
        int begin1=pv[i];
        int end1=pv[i+1];
        int begin2=qv[i];
        int end2=qv[i+1];

        table->setItem(partRowcount, 0, new QTableWidgetItem(se->getID()));
        str  = QString::number(begin2) + "-"+QString::number(end2);
        table->setItem(partRowcount, 1, new QTableWidgetItem(str));

        str  = QString::number(begin1) + "-"+QString::number(end1);
        table->setItem(partRowcount, 2, new QTableWidgetItem(str));

        double res = computeDFD_new(begin1, end1, begin2, end2);

        str  = QString::number(res);
        table->setItem(partRowcount, 3, new QTableWidgetItem(str));
        partRowcount ++;
    }

}


void SearchWin::calSecPart()
{

    QProgressDialog progress(tr("正在计算轨迹数据，请稍候..."),
                             tr("取消"),
                             0, tracs->length());
    progress.setWindowModality(Qt::WindowModal);

    for (int i = 0; i < tracs->length(); i++)
    {
        QVector<QVector<int> >qc;
        Sequence sf;
        sf = id_seq_map[tracs->at(i)];
        qc = getSimplify(input,&sf);

        if (sf.hasTime() && input->hasTime())
        {
            fillPartTable(ui->searchTable_time_part,
                          qc, &sf);
        }
        else if (!sf.hasTime() && !input->hasTime())
        {
            fillPartTable(ui->searchTable_common_part,
                          qc, &sf);
        }
        progress.setValue(i);
        if (progress.wasCanceled()) {
            break;
        }
    }
    progress.setValue(tracs->length());
}

void SearchWin::drawSeq()
{
    int seqToDraw = 3;
    QVector<Sequence> temp;
    temp.append(*input);
    if(tracs->length() == 0)
    {
        return;
    }
    if (time)
    {
        double minRes = (ui->searchTable_time->item(0,2)->text()).toDouble();
        for(int i =  0; i < tracs->length(); i++)
        {
            QString id = ui->searchTable_time->item(i,0)->text();
            double res = (ui->searchTable_time->item(i,2)->text()).toDouble();
            if(res > minRes)
            {
                break;
            }
            temp.append(id_seq_map[id]);
            if (temp.length() > seqToDraw)
            {
                break;
            }
        }
    }
    else
    {
        double minRes = (ui->searchTable_common->item(0,2)->text()).toDouble();
        for(int i =  0; i < tracs->length(); i++)
        {
            QString id = ui->searchTable_common->item(i,0)->text();
            double res = (ui->searchTable_common->item(i,2)->text()).toDouble();
            if(res > minRes)
            {
                break;
            }
            temp.append(id_seq_map[id]);
            if (temp.length() > seqToDraw)
            {
                break;
            }
        }
    }

    ui->searchMap->initJS();
    ui->searchMap->showPoints(true);
    ui->searchMap->showTimes(true);
    ui->searchMap->setCentralPoint(getCenterPoint(temp), getZoom(temp));
    ui->searchMap->drawSequences(temp, coincide);
    ui->searchMap->reload();
}

void SearchWin::drawPart()
{
    int seqToDraw = 3;
    QVector<Sequence> temp;
    temp.append(*input);
    ui->searchMap->initJS();
    ui->searchMap->showPoints(true);
    ui->searchMap->showTimes(true);

    if (time)
    {
        double minRes = (ui->searchTable_time_part->item(0,3)->text()).toDouble();
        for(int i =  0; i < partRowcount - 1; i++)
        {
            QString id = ui->searchTable_time_part->item(i,0)->text();
            QString idn = ui->searchTable_time_part->item(i+1, 0)->text();
            double res = (ui->searchTable_time_part->item(i,3)->text()).toDouble();

            if(res > minRes)
            {
                break;
            }
            if (id != idn)
            {
                temp.append(id_seq_map[id]);
            }
            QString index1 = ui->searchTable_time_part->item(i,1)->text();
            QString index2 = ui->searchTable_time_part->item(i,2)->text();

            int S = index1.split("-")[0].toInt();
            int E = index1.split("-")[1].toInt();
            ui->searchMap->highLightPart(&(id_seq_map[id]),S,E,3,10);
            int inputS = index2.split("-")[0].toInt();
            int inputE = index2.split("-")[1].toInt();
            ui->searchMap->highLightPart(input,inputS,inputE,3,10);
            if (temp.length() > seqToDraw)
            {
                break;
            }
        }
    }
    else
    {
        double minRes = (ui->searchTable_common_part->item(0,3)->text()).toDouble();
        for(int i =  0; i < partRowcount - 1; i++)
        {
            QString id = ui->searchTable_common_part->item(i,0)->text();
            QString idn = ui->searchTable_common_part->item(i+1, 0)->text();
            double res = (ui->searchTable_common_part->item(i,3)->text()).toDouble();

            if(res > minRes)
            {
                break;
            }
            if (id != idn)
            {
                temp.append(id_seq_map[id]);
            }
            QString index1 = ui->searchTable_common_part->item(i,1)->text();
            QString index2 = ui->searchTable_common_part->item(i,2)->text();

            int S = index1.split("-")[0].toInt();
            int E = index1.split("-")[1].toInt();
            ui->searchMap->highLightPart(&(id_seq_map[id]),S,E,3,10);
            int inputS = index2.split("-")[0].toInt();
            int inputE = index2.split("-")[1].toInt();
            ui->searchMap->highLightPart(input,inputS,inputE,3,10);
            if (temp.length() > seqToDraw)
            {
                break;
            }
        }
    }


    ui->searchMap->setCentralPoint(getCenterPoint(temp), getZoom(temp));
    ui->searchMap->drawSequences(temp, coincide);
    ui->searchMap->reload();
}

void SearchWin::drawPoints()
{
    int seqToDraw = 3;
    QVector<Sequence> temp;
    temp.append(*input);
    ui->searchMap->initJS();
    ui->searchMap->showPoints(true);
    ui->searchMap->showTimes(true);

    if (time)
    {
        double minRes = (ui->searchTable_time_point->item(0,3)->text()).toDouble();
        for(int i =  0; i < rowcount - 1; i++)
        {
            QString id = ui->searchTable_time_point->item(i,0)->text();
            QString idn = ui->searchTable_time_point->item(i+1, 0)->text();
            double res = (ui->searchTable_time_point->item(i,3)->text()).toDouble();
            if(res > minRes)
            {
                break;
            }
            if (id != idn)
            {
                temp.append(id_seq_map[id]);
            }

            QString index1 = ui->searchTable_time_point->item(i,1)->text();
            QString index2 = ui->searchTable_time_point->item(i,2)->text();

            int S = index1.toInt();
            ui->searchMap->drawPoint(id_seq_map[id].pts[S],"p"+
                                     id_seq_map[id].getID()+QString::number(i),6,true);

            int inputS = index2.toInt();
            ui->searchMap->drawPoint(input->pts[inputS],
                                     input->getID()+QString::number(i),0,true);

            if (temp.length() > seqToDraw)
            {
                break;
            }
        }
    }
    else
    {
        double minRes = (ui->searchTable_common_point->item(0,3)->text()).toDouble();
        for(int i =  0; i < rowcount - 1; i++)
        {
            QString id = ui->searchTable_common_point->item(i,0)->text();
            QString idn = ui->searchTable_common_point->item(i+1, 0)->text();
            double res = (ui->searchTable_common_point->item(i,3)->text()).toDouble();
            if(res > minRes)
            {
                break;
            }
            if (id != idn)
            {
                temp.append(id_seq_map[id]);
            }

            QString index1 = ui->searchTable_common_point->item(i,1)->text();
            QString index2 = ui->searchTable_common_point->item(i,2)->text();

            int S = index1.toInt();
            ui->searchMap->drawPoint(id_seq_map[id].pts[S],"p"+
                                     id_seq_map[id].getID()+QString::number(i),6,true);

            int inputS = index2.toInt();
            ui->searchMap->drawPoint(input->pts[inputS],
                                     input->getID()+QString::number(i),0,true);

            if (temp.length() > seqToDraw)
            {
                break;
            }
        }
    }

    ui->searchMap->setCentralPoint(getCenterPoint(temp), getZoom(temp));
    ui->searchMap->drawSequences(temp, coincide);
    ui->searchMap->reload();


}


void SearchWin::searchPoint()
{
    QVector<PointCompare> pVec;
    for (int i = 0; i < tracs->length(); i++)
    {
        Sequence sf;
        sf = id_seq_map[tracs->at(i)];
        if (sf.hasTime() && input->hasTime())
        {
            pVec = getNearestPoint(input, &sf);
            fillPointTable(ui->searchTable_time_point, pVec, &sf);
        }
        else if (!sf.hasTime() && !input->hasTime())
        {
            pVec = getNearestPoint(input, &sf);
            fillPointTable(ui->searchTable_common_point, pVec, &sf);
        }
    }
}

void SearchWin::loadIntoMem()
{
    string tb = "importtest";
    seqs.append(*input);
    id_seq_map.insert(input->getID(), *input);

    for (int i = 0; i < tracs->length(); i++)
    {
        //将轨迹存入内存，便于后期使用
        Sequence sf;
        db->getSequenceByID(tb,&sf,QString((*tracs)[i]).toStdString());
        seqs.append(sf);
        id_seq_map.insert(sf.getID(), sf);
    }
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
    header << tr("轨迹ID")
           << tr("点个数")
           << tr("Frechet Distance")
           << tr("相似度");
    ui->searchTable_common->setHorizontalHeaderLabels(header);
    ui->searchTable_time->setHorizontalHeaderLabels(header);
    if (tracs->length() == 0)
    {
        ui->searchTable_common->clearContents();
        ui->searchTable_time->clearContents();
    }
    else {
        pos_c = 0;
        pos_t = 0;
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
        for(int i = 0; i < tracs->length(); i++)
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
//    time = false;
    seqFlag = false;
    partFlag = false;
    pointFlag  = false;

    coincide.clear();
    seqs.clear();
    id_seq_map.clear();
    ui->searchtabWidget->setCurrentIndex(0);
    ui->searchStackedWidget->setCurrentIndex(0);
    ui->searchStackedWidget_time->setCurrentIndex(0);
}

void SearchWin::clearInput()
{
    if (input->getNum() != 0)
    {
        delete input;
        input = new Sequence();
    }
}


void SearchWin::openFile()
{

    clearInput();
    QString file_name = QFileDialog::getOpenFileName(NULL,
                        tr("Open File"),
                        "",
                        "CSV Files(*.csv)",
                        0);
    if (!file_name.isNull())
    {
        qDebug()<<file_name;
    }
    else {
        return;
    }
    string fileName = file_name.toLocal8Bit().data();
    ifstream fin(fileName.c_str());
    Csv csv(fin);
    getSquFromFile(&csv,input);
    if(input->getNum() == 0)
    {
        return;
    }
    ui->searchPathEdit->setText(file_name);
    init();
    if (input->hasTime())
    {
        time = true;
    }
    else
    {
        time = false;
    }
    refreshTable();
}

void SearchWin::rankPartOfSeq()
{
    if (input == NULL)
    {
        return;
    }
    if (input->getNum() == 0)
    {
        return;
    }

    if (time)
    {
        ui->searchtabWidget->setCurrentIndex(1);
    }
    else
    {
        ui->searchtabWidget->setCurrentIndex(0);
    }
    ui->searchStackedWidget->setCurrentIndex(2);
    ui->searchStackedWidget_time->setCurrentIndex(2);

    if (partFlag)
    {
        drawPart();
        return;
    }

    partRowcount = 0;
    this->ui->searchMap->initJS();
    this->ui->searchMap->setDefaultCentralPt();
    this->ui->searchMap->reload();

    if (seqs.length() == 0)
    {
        loadIntoMem();

    }
    if (coincide.length() == 0)
    {
        searchSeq();
        sortSeqTable();
    }
    calSecPart();
    sortPartTable();
    drawPart();
    partFlag = true;
}

void SearchWin::rankSeqClicked()
{
    if (input == NULL)
    {
        return;
    }
    if (input->getNum() == 0)
    {
        drawSeq();
        return;
    }
    if (time)
    {
        ui->searchtabWidget->setCurrentIndex(1);
    }
    else
    {
        ui->searchtabWidget->setCurrentIndex(0);
    }
    ui->searchStackedWidget->setCurrentIndex(0);
    ui->searchStackedWidget_time->setCurrentIndex(0);
    if (seqFlag)
    {
        drawSeq();
        return;
    }

    this->ui->searchMap->initJS();
    this->ui->searchMap->setDefaultCentralPt();
    this->ui->searchMap->reload();

    if (seqs.length() == 0)
    {
        loadIntoMem();
    }

    if (coincide.length() == 0)
    {
        searchSeq();
        sortSeqTable();
    }
    drawSeq();
    if (coincide.length() != 0)
    {
        QMessageBox::information(NULL, "提示", "发现与输入轨迹完全重合的轨迹，这些轨迹将不会绘制在地图中", QMessageBox::Yes, QMessageBox::Yes);
    }
    showPartofSeq();
    seqFlag = true;

}

void SearchWin::rankSeqPointClicked()
{

    if (input == NULL)
    {
        return;
    }
    if (input->getNum() == 0)
    {
        return;
    }

    if (time)
    {
        ui->searchtabWidget->setCurrentIndex(1);
    }
    else
    {
        ui->searchtabWidget->setCurrentIndex(0);
    }
    ui->searchStackedWidget->setCurrentIndex(1);
    ui->searchStackedWidget_time->setCurrentIndex(1);

    if (pointFlag)
    {   \
        drawPoints();
        return;
    }
    rowcount = 0;
    this->ui->searchMap->initJS();
    this->ui->searchMap->setDefaultCentralPt();
    this->ui->searchMap->reload();
    if (seqs.length() == 0)
    {
        loadIntoMem();
    }

    if (coincide.length() == 0)
    {
        searchSeq();
        sortSeqTable();
    }
    searchPoint();
    sortPointTable();
    drawPoints();
    showPartofSeq();

    pointFlag = true;
}


