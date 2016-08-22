#include "searchwin.h"
#include "ui_mainwindow.h"
#include <QProgressDialog>


SearchWin::SearchWin(Ui::MainWindow *ui,DataBase *db1)
{
    this->ui = ui;
    db = db1;

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

//    db = new DataBase(1);
    detWin = new detailWin();

    ui->searchMap->initJS();
    ui->searchMap->reload();

    statusPart=0;
    statusPoint=0;
    statusSeq=0;

    init();
    initSig();


    initTable(ui->searchTable_common);
    initTable(ui->searchTable_common_part);
    initTable(ui->searchTable_common_point);
    initTable(ui->searchTable_time);
    initTable(ui->searchTable_time_part);
    initTable(ui->searchTable_time_point);
//    initTable(ui->searchNewSelected);
//    initTable(ui->searchNewTime);
    initSeqPartTable(ui->searchTable_common_part);
    initSeqPartTable(ui->searchTable_time_part);
    initPointTable(ui->searchTable_common_point);
    initPointTable(ui->searchTable_time_point);

    initNew(ui->searchNewTime);
    initNew(ui->searchNewSelected);

    ui->searchStackedWidget->setCurrentIndex(0);
    ui->searchStackedWidget_time->setCurrentIndex(0);
    //refreshTable();
}

void SearchWin::setTracs(QStringList *tracs)
{
    this->tracs = tracs;
}


void SearchWin::setDB(DataBase *db1)
{
    db = db1;
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
    //    ui->searchTable_common->addAction(detailAct);
    //    ui->searchTable_time->addAction(detailAct);
    table->addAction(detailAct);
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
   // table->addAction(detailAct);
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
  //  table->addAction(detailAct);
}

void SearchWin::initNew(QTableWidget *table){
//    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setColumnCount(3);
//    table->verticalHeader()->hide();
    table->setContextMenuPolicy(Qt::ActionsContextMenu);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setRowCount(ROW_NUM);
    table->clearContents();
    QStringList header;
    header << "轨迹ID"
           << "轨迹ID"
           << "Frechet Distance";
    table->setHorizontalHeaderLabels(header);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->addAction(backAct);
}

void SearchWin::searchSeq()
{
    double dfDis;
    double maxDis = 0;
    int c = 0;
    int t = 0;
    Sequence sf;
    QTableWidgetItem *tItem;
    Core core;
    QProgressDialog* pro= new QProgressDialog(tr("正在搜索，请稍候..."), tr("取消"),
                                 0,tracs->length());
    pro->setWindowModality(Qt::WindowModal);
    pro->setWindowTitle(tr("Please Wait"));

    pro->show();
    qApp->processEvents();
    for (int i = 0; i < tracs->length(); i++)
    {
        pro->setValue(i);
        if(pro->wasCanceled()){
            break;
        }
        sf = id_seq_map[tracs->at(i)];

        tItem = new QTableWidgetItem();

        if((input->hasTime() && sf.hasTime())||((!input->hasTime()) && (!sf.hasTime()))){
            dfDis = core.computeDiscreteFrechet(input,&sf);
        }
        else {
            continue;
        }

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
    }
    pro->setValue(tracs->length());

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
//    ui->searchNewSelected->sortItems(2, Qt::AscendingOrder);
    ui->searchTable_time->sortItems(2, Qt::AscendingOrder);
//    ui->searchNewTime->sortItems(2, Qt::AscendingOrder);
    QAbstractItemModel *model;
    QModelIndex index1;
    QString id1;
    if(tracs->length()>5000&&storeId.length()==0){
        for(int i=0;i<100;i++){
            model = ui->searchTable_common->model();
            index1 = model->index(i,0);
            id1 = model->data(index1).toString();
            storeId.append(id1);
        }
    }else if(tracs->length()>500&&storeId.length()==0){
        for(int i=0;i<50;i++){
            model = ui->searchTable_common->model();
            index1 = model->index(i,0);
            id1 = model->data(index1).toString();
            storeId.append(id1);
        }
    }else if((tracs->length()>50)&&storeId.length()==0){
        for(int i=0;i<30;i++){
            model = ui->searchTable_common->model();
            index1 = model->index(i,0);
            id1 = model->data(index1).toString();
            storeId.append(id1);
        }
    }else if(storeId.length()==0){
        for(int i=0;i<tracs->length()/2;i++){
            model = ui->searchTable_common->model();
            index1 = model->index(i,0);
            id1 = model->data(index1).toString();
            storeId.append(id1);
        }
    }
}

void SearchWin::initSig()
{
    connect(ui->searchStartBtn, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->calSeqPartBtn, SIGNAL(clicked()), this, SLOT(rankPartOfSeq()));
    connect(ui->calSeqBtn, SIGNAL(clicked()), this, SLOT(rankSeqClicked()));
    connect(ui->calSeqPointBtn, SIGNAL(clicked()), this, SLOT(rankSeqPointClicked()));
    detailAct = new QAction(tr("详情"),this);
    connect(detailAct, SIGNAL(triggered()), this, SLOT(showDetail()));
    backAct=new QAction(tr("返回"),this);
    connect(backAct, SIGNAL(triggered()), this, SLOT(back()));
}

void SearchWin::showDetail(){
    selectedSeqS= getCurrentSeq();
    if (selectedSeqS.length() == 0) {
        return;
    }
    detWin->setWindowModality(Qt::ApplicationModal);
    detWin->showSeq(selectedSeqS[0]);

    detWin->show();
}

void SearchWin::back(){

    init();
    refreshTable();
    ui->searchMap->initJS();
    ui->searchMap->setDefaultCentralPt();
    ui->searchMap->reload();
}

void SearchWin::searchSelectedSeqs(){
    double dfDis;
  //  double maxDis = 0;
    int c = 0;
    int t = 0;

    QProgressDialog progress(tr("正在计算轨迹数据，请稍候..."),
                             tr("取消"),
                             0, selectedSeqS.length());
    progress.setWindowModality(Qt::WindowModal);
    //progress.show();

    Core core;
    for (int i = 0; i < selectedSeqS.length()-1; i++)
    {
//        Sequence sf;
//        sf = selectedSeqS[i];

       // QTableWidgetItem *tItem = new QTableWidgetItem();
        progress.setValue(i);
        if (progress.wasCanceled()) {
            break;
        }
        for(int j=i+1;j< selectedSeqS.length();j++){

            dfDis = core.computeDiscreteFrechet(&selectedSeqS[j],&selectedSeqS[i]);
//            if (dfDis == 0)
//            {
//                coincide << selectedSeqS[i].getID();
//            }
            //tItem->setData(Qt::DisplayRole,dfDis);

            if (selectedSeqS[i].hasTime())
            {
                ui->searchNewTime->setItem(t,0,new QTableWidgetItem(selectedSeqS[i].getID()));
                ui->searchNewTime->setItem(t,1,new QTableWidgetItem(selectedSeqS[j].getID()));
                ui->searchNewTime->setItem(t,2,new QTableWidgetItem(QString::number(dfDis)));
                t++;
            }
            else if (!selectedSeqS[i].hasTime())
            {
                //ui->searchTable_common->setItem(c,2,tItem);
                ui->searchNewSelected->setItem(c,0,new QTableWidgetItem(selectedSeqS[i].getID()));
                ui->searchNewSelected->setItem(c,1,new QTableWidgetItem(selectedSeqS[j].getID()));
                ui->searchNewSelected->setItem(c,2,new QTableWidgetItem(QString::number(dfDis)));
                c++;
            }
//            if (dfDis >= maxDis)
//            {
//                maxDis = dfDis;
//            }
        }

    }
    progress.setValue(selectedSeqS.length());

    if ( c != 0)
    {
        time = false;
        ui->searchtabWidget->setCurrentIndex(0);
        ui->searchNewSelected->sortItems(2,Qt::AscendingOrder);
//        for (int i = 0; i < c; i++)
//        {
//            QTableWidgetItem *tItem = new QTableWidgetItem();
//            double dis = ui->searchTable_common->item(i, 3)->text().toDouble();
//            //double dis = 0;
//            double percent = (maxDis - dis)/maxDis * 100;

//            if (percent < 0)
//            {
//                percent = 0;
//            }
//            tItem->setData(Qt::DisplayRole,
//                           QString::number(percent) + "%");
//            ui->searchTable_common->setItem(i,3,tItem);
//        }
    }
    else if (t != 0)
    {
        time = true;
        ui->searchtabWidget->setCurrentIndex(1);
        ui->searchNewTime->sortItems(2,Qt::AscendingOrder);
//        for (int i = 0; i < t; i++)
//        {
//            QTableWidgetItem *tItem = new QTableWidgetItem();
//            //double dis = ui->searchTable_time->item(i, 2)->text().toDouble();
//            double dis = 0;
//            double percent = (maxDis - dis)/maxDis * 100;
//            if (percent < 0)
//            {
//                percent = 0;
//            }
//            tItem->setData(Qt::DisplayRole,
//                           QString::number(percent) + "%");
//            ui->searchTable_time->setItem(i,3,tItem);
//        }
    }
 //   selectedSeqS.clear();
}

QVector<Sequence> SearchWin::getCurrentSeq(){
    int currentIndex=ui->searchtabWidget->currentIndex();
    QVector<Sequence> tempSeq;
    QModelIndexList indexes;
    if(currentIndex){
        indexes = ui->searchTable_time->selectionModel()->selectedIndexes();
        if(statusPart){
            indexes = ui->searchTable_time_part->selectionModel()->selectedIndexes();
        }
        if(statusPoint){
            indexes = ui->searchTable_time_point->selectionModel()->selectedIndexes();
        }
        if(statusSeq){
            indexes = ui->searchTable_time->selectionModel()->selectedIndexes();
        }
    }else{
        indexes = ui->searchTable_common->selectionModel()->selectedIndexes();
        if(statusPart){
            indexes = ui->searchTable_common_part->selectionModel()->selectedIndexes();
        }
        if(statusPoint){
            indexes = ui->searchTable_common_point->selectionModel()->selectedIndexes();
        }
        if(statusSeq){
            indexes = ui->searchTable_common->selectionModel()->selectedIndexes();
        }
    }
    tName="importtest";
    if (indexes.length() != 0) {
        for (int i = 0; i < indexes.length(); i=i+4) {
            Sequence *temp = new Sequence();
            int curRow = indexes[i].row();
            QModelIndex indextemp;
            if(currentIndex){
                indextemp = ui->searchTable_time->model()->index(curRow,0);
                if(statusPart){
                    indextemp = ui->searchTable_time_part->model()->index(curRow,0);
                }
                if(statusPoint){
                    indextemp  = ui->searchTable_time_point->model()->index(curRow,0);
                }
                if(statusSeq){
                    indextemp  = ui->searchTable_time->model()->index(curRow,0);
                }
            }else{
                indextemp = ui->searchTable_common->model()->index(curRow,0);
                if(statusPart){
                    indextemp  = ui->searchTable_common_part->model()->index(curRow,0);
                }
                if(statusPoint){
                    indextemp  = ui->searchTable_common_point->model()->index(curRow,0);
                }
                if(statusSeq){
                    indextemp = ui->searchTable_common->model()->index(curRow,0);
                }
            }

            string id = indextemp.data().toString().toStdString();
            if (id != "") {
                db->getSequenceByID(tName,temp,id);
                tempSeq.append(*temp);
            }
        }
    }
    return tempSeq;

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
    Core core;
    core.initP_Q(input, se);
    core.initMemSpace(input, se);
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

        double res = core.computeDFD_new(begin1, end1, begin2, end2);

        str  = QString::number(res);
        table->setItem(partRowcount, 3, new QTableWidgetItem(str));
        partRowcount ++;
    }

}

void SearchWin::calSecPart()
{

    QProgressDialog progress(tr("正在计算轨迹段数据，请稍候..."),
                             tr("取消"),
                             0, tracs->length());
    progress.setWindowModality(Qt::WindowModal);
    //  #pragma omp parallel for
    QVector<QVector<int> >qc;
    Sequence sf;

    Core core;
    core.tracsLen=tracs->length();

    if(!input->hasTime()){
        if(storeId.length()!=0){
            progress.setMaximum(storeId.length());
            for (int i = 0; i <storeId.length(); i++)
            {
             //   omp_set_lock(&lock);
              sf = id_seq_map[storeId[i]];

                if (!sf.hasTime())
                {
                    qc = core.getSimplify(input,&sf);
                    fillPartTable(ui->searchTable_common_part,
                                  qc, &sf);
                }
                progress.setValue(i);
                if (progress.wasCanceled()) {
                    break;
                }
//                std::cout<<i<<endl;
               // omp_unset_lock(&lock);
            }
            progress.setValue(storeId.length());
        }else{
            if(!seqFlag){
                searchSeq();
                sortSeqTable();
            }
            progress.setMaximum(storeId.length());
            for (int i = 0; i <storeId.length(); i++)
            {
             //   omp_set_lock(&lock);

                sf = id_seq_map[storeId[i]];

               if (!sf.hasTime() && !input->hasTime())
                {
                    qc = core.getSimplify(input,&sf);
                    fillPartTable(ui->searchTable_common_part,
                                  qc, &sf);
                }
                std::cout<<i<<endl;
                progress.setValue(i);
                if (progress.wasCanceled()) {
                    break;
                }
               // omp_unset_lock(&lock);
            }
            progress.setValue(storeId.length());
        }
    }else{
            progress.setMaximum(tracs->length());
            for (int i = 0; i <tracs->length(); i++)
            {
             //   omp_set_lock(&lock);
               sf = id_seq_map[tracs->at(i)];


                if (sf.hasTime())
                {
                    qc = core.getSimplify(input,&sf);
                    fillPartTable(ui->searchTable_time_part,
                                  qc, &sf);
                }
                progress.setValue(i);
                if (progress.wasCanceled()) {
                    break;
                }
                std::cout<<i<<endl;
               // omp_unset_lock(&lock);
            }
            progress.setValue(tracs->length());
        }
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
            if(!(ui->searchTable_time->item(i,0)))
            {
                break;
            }
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
            if(!(ui->searchTable_common->item(i,0)))
            {
                break;
            }
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

void SearchWin::drawNewSeq(){
    int currentIndex=ui->searchtabWidget->currentIndex();

    QVector<Sequence> temp;

    string id1;
    string id2;
    double dis;

    if(currentIndex){
        id1= ui->searchNewTime->item(0,0)->text().toStdString();
        id2= ui->searchNewTime->item(0,1)->text().toStdString();
        dis= ui->searchNewTime->item(0,2)->text().toDouble();

    }else{
        id1= ui->searchNewSelected->item(0,0)->text().toStdString();
        id2= ui->searchNewSelected->item(0,1)->text().toStdString();
        dis= ui->searchNewSelected->item(0,2)->text().toDouble();
    }

    Sequence sf1;
    Sequence sf2;
    db->getSequenceByID(tName,&sf1,id1);
    db->getSequenceByID(tName,&sf2,id2);

    temp.append(sf1);
    temp.append(sf2);

    ui->searchMap->initJS();
    ui->searchMap->showPoints(true);
    ui->searchMap->showTimes(true);
    ui->searchMap->setCentralPoint(getCenterPoint(temp), getZoom(temp));
    ui->searchMap->drawSequences(temp);
    ui->searchMap->reload();

    if(!dis){
      QMessageBox::information(NULL, "提示", "两条轨迹完全重合", QMessageBox::Yes, QMessageBox::Yes);
    }
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
            ui->searchMap->drawPoint((id_seq_map[id]).pts[S],"p"+
                                     (id_seq_map[id]).getID()+QString::number(i),6,true);


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
            ui->searchMap->drawPoint((id_seq_map[id]).pts[S],"p"+
                                     (id_seq_map[id]).getID()+QString::number(i),6,true);

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
    Core core;
    QProgressDialog* pro= new QProgressDialog(tr("正在搜索计算轨迹点，请稍候..."), tr("取消"),
                                 0,tracs->length());
    pro->setWindowModality(Qt::WindowModal);
    pro->setWindowTitle(tr("Please Wait"));

    pro->show();
    qApp->processEvents();
    for (int i = 0; i < tracs->length(); i++)
    {
        Sequence sf;
//        if(id_seq_map[tracs->at(i)]->)
        sf = id_seq_map[tracs->at(i)];
        pro->setValue(i);
        if(pro->wasCanceled()){
            break;
        }
        if (sf.hasTime() && input->hasTime())
        {
            pVec = core.getNearestPoint(input, &sf);
            fillPointTable(ui->searchTable_time_point, pVec, &sf);
        }
        else if (!sf.hasTime() && !input->hasTime())
        {
            pVec = core.getNearestPoint(input, &sf);
            fillPointTable(ui->searchTable_common_point, pVec, &sf);
        }
    }
    pro->setValue(tracs->length());
}

void SearchWin::loadIntoMem()
{      
    string tb = "importtest";
    QVector<Sequence>s=db->getAllSequences(tb);
    for(int i=0;i<s.length();i++){
        id_seq_map.insert(s[i].getID(),s[i]);
    }
    id_seq_map.insert(input->getID(),*input);
}

void SearchWin::refreshTable()
{
    initSeqPartTable(ui->searchTable_common_part);
    initSeqPartTable(ui->searchTable_time_part);
    initPointTable(ui->searchTable_common_point);
    initPointTable(ui->searchTable_time_point);
    initNew(ui->searchNewSelected);
    initNew(ui->searchNewTime);

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
//    seqs.clear();
    id_seq_map.clear();
    ui->searchtabWidget->setCurrentIndex(0);
    ui->searchStackedWidget->setCurrentIndex(0);
    ui->searchStackedWidget_time->setCurrentIndex(0);
}

void SearchWin::initWithOutClear()
{
    //    time = false;
    seqFlag = false;
    partFlag = false;
    pointFlag  = false;

    coincide.clear();
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
    storeId.clear();
    seqFlag=false;
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
    try
    {
        getSquFromFile(&csv,input);
    }
    catch(int i)
    {
        QMessageBox::information(NULL, "Error 错误代码" + QString::number(i), "时间格式错误,格式类似20160601 13:00:11", QMessageBox::Yes, QMessageBox::Yes);
        delete input;
        input = new Sequence();
        ui->searchPathEdit->clear();
        fin.close();
        return;
    }

    if(input->getNum() == 0)
    {
        return;
    }
    ui->searchPathEdit->setText(file_name);
    initWithOutClear();
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

   // pro->setWindowTitle(tr("Please Wait"));

    statusPart=1;
    statusPoint=0;
    statusSeq=0;

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
    ui->searchStackedWidget_time->setCurrentIndex(3);

    if (partFlag)
    {
        drawPart();
        return;
    }

    partRowcount = 0;
    this->ui->searchMap->initJS();
    this->ui->searchMap->setDefaultCentralPt();
    this->ui->searchMap->reload();

    if (id_seq_map.empty())
    {
        loadIntoMem();

    }
    if (coincide.length() == 0)
    {
        //searchSeq();
        //sortSeqTable();
    }
    calSecPart();
    sortPartTable();
    drawPart();
    partFlag = true;
}

void SearchWin::rankSeqClicked()
{

    statusPart=0;
    statusPoint=0;
    statusSeq=1;

    if(input->getNum()==0){
        selectedSeqS= getCurrentSeq();
    }

    if(selectedSeqS.length()>1){
        init();
        refreshTable();
    }

    if (input == NULL&&selectedSeqS.length()<=1)
    {
        return;
    }

    if (input->getNum() == 0&&selectedSeqS.length()<=1)
    {
        //     drawSeq();
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

    if(selectedSeqS.length()>1){
        ui->searchStackedWidget->setCurrentIndex(3);
        ui->searchStackedWidget_time->setCurrentIndex(2);
    }else{
        ui->searchStackedWidget->setCurrentIndex(0);
        ui->searchStackedWidget_time->setCurrentIndex(0);
    }

    if (seqFlag)
    {
        drawSeq();
        return;
    }

    this->ui->searchMap->initJS();
    this->ui->searchMap->setDefaultCentralPt();
    this->ui->searchMap->reload();



    if (coincide.length() == 0)
    {
        if(selectedSeqS.length()>1){
            searchSelectedSeqs();
            drawNewSeq();
        }else{
            if (id_seq_map.empty())
            {
                loadIntoMem();
            }
            searchSeq();
            sortSeqTable();
            drawSeq();
        }

    }

    if (coincide.length() != 0&&(selectedSeqS.length()<=1))
    {
        QMessageBox::information(NULL, "提示", "发现与输入轨迹完全重合的轨迹，这些轨迹将不会绘制在地图中", QMessageBox::Yes, QMessageBox::Yes);
    }


    seqFlag = true;
    selectedSeqS.clear();
}

void SearchWin::rankSeqPointClicked()
{
    statusPart=0;
    statusPoint=1;
    statusSeq=0;

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
    if (id_seq_map.empty())
    {
        loadIntoMem();
    }

//    if (coincide.length() == 0)
//    {
//        searchSeq();
//        //sortSeqTable();
//    }
    searchPoint();
    sortPointTable();
    drawPoints();

    pointFlag = true;
}


