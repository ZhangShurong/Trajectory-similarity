#include "mainwin.h"
#include "ui_mainwin.h"

mainWin::mainWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainWin)
{
    ui->setupUi(this);
    tName = "importtest";
    initMap();
    initAction();
    initTable();
    initCSS();
    initSig();
    initCan();
    detWin = new detailWin();
    calWin = new calPanel();
    setWin = new mainSettingWin();
    searchMode(false);
}

void mainWin::setDb(DataBase *database)
{
    db = database;
}

QVector<Sequence> mainWin::getCurrentSeqs()
{
    QVector<Sequence> tempSeq;
    QModelIndexList indexes = ui->mainTable->selectionModel()->selectedIndexes();
    if (indexes.length() != 0)
    {
        for (int i = 0; i < indexes.length();i=i+4)
        {
            Sequence *temp = new Sequence();
            int curRow = indexes[i].row();
            QModelIndex indextemp = ui->mainTable->model()->index(curRow,0);
            string id = indextemp.data().toString().toStdString();
            if (id != "")
            {
                db->getSequenceByID(tName,temp,id);
               // ui->canvasWidget->setSequences(temp,1);
                tempSeq.append(*temp);
            }
        }
    }
    return tempSeq;
}


mainWin::~mainWin()
{
    delete ui;
    delete db;
    delete map;
    delete can;
  //CSS
    delete cssFile;
    delete strCSS;
   // calWindow *calWin;
    delete calWin;
    delete detWin;


//Table action
    delete deleteAct;
    delete searchAct;
    delete showInMapAct;
    delete detailAct;
    delete setWin;
}

void mainWin::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;
        button = QMessageBox::question(this, tr("退出程序"),
            QString(tr("是否结束操作退出?")),
            QMessageBox::Yes | QMessageBox::No);

        if (button == QMessageBox::No) {
            event->ignore();  //忽略退出信号，程序继续运行
        }
        else if (button == QMessageBox::Yes) {
            event->accept();  //接受退出信号，程序退出
        }
}

void mainWin::initAction()
{
    deleteAct = new QAction(tr("Delete"), this);
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteOneAct()));
    searchAct = new QAction(tr("Search"),this);
    connect(searchAct, SIGNAL(triggered()), this, SLOT(searchSlot_R()));
    showInMapAct = new QAction(tr("ShowInMap"),this);
    connect(showInMapAct, SIGNAL(triggered()), this, SLOT(showInMapSlot_R()));
    detailAct = new QAction(tr("Detail"),this);
    connect(detailAct, SIGNAL(triggered()), this, SLOT(detailSlot_R()));
}

void mainWin::initCSS()
{
    cssFile = new QFile(":/css/mainWin.qss");
    cssFile->open(QIODevice::ReadOnly);
    strCSS = new QString(QLatin1String(cssFile->readAll()));
    this->setStyleSheet(*strCSS);
    cssFile->close();
}

void mainWin::initTable()
{
    ui->mainTable->horizontalHeader()->setStretchLastSection(true);
    ui->mainTable->verticalHeader()->hide();
    ui->mainTable->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->mainTable->addAction(deleteAct);
    ui->mainTable->addAction(searchAct);
    ui->mainTable->addAction(showInMapAct);
    ui->mainTable->addAction(detailAct);

    ui->mainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->mainTable->setColumnCount(4);
    ui->mainTable->setRowCount(1000);
    ui->mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    refreshTable();
}

void mainWin::refreshTable()
{
    ui->mainTable->clearContents();
    QStringList header;
    header << tr("ID")
           << tr("PointNumber")
           << tr("Frechet Distance")
           << tr("Similarity");
    ui->mainTable->setHorizontalHeaderLabels(header);
    QStringList *tracs;
    tracs = db->getAllTracID(tName);
    if (tracs->length() == 0)
        ui->mainTable->clear();
    else {
        for(int i = 0; i < tracs->length();i++)
        {

            QString temp = (*tracs)[i];
            ui->mainTable->setItem(i,0, new QTableWidgetItem(temp));
            ui->mainTable->setItem(i,1,
                               new QTableWidgetItem(QString::number(db->getPointNumByID(temp.toStdString()))));
        }
    }
    delete tracs;
}

void mainWin::initSig()
{
    connect(ui->ImportOneAct, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui->AboutQtAct, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->SearchAct, SIGNAL(triggered()), this, SLOT(searchInDB()));
    connect(ui->ImportFilesAct, SIGNAL(triggered()), this, SLOT(openFilesSlot()));
    connect(ui->ClearDBAct, SIGNAL(triggered()), this, SLOT(clearDB()));
    connect(ui->ViewCanvasAct, SIGNAL(triggered()), this, SLOT(paint()));
    connect(ui->ViewMapAct, SIGNAL(triggered()), this, SLOT(showMap()));
    connect(ui->CalculateAct, SIGNAL(triggered()), this, SLOT(calActSlot()));
    connect(ui->mainTable, SIGNAL(clicked(QModelIndex)), this, SLOT(clickTBSlot(QModelIndex)));
    connect(ui->importDataBtn, SIGNAL(clicked()), this, SLOT(openFilesSlot()));
    connect(ui->searchBtn, SIGNAL(clicked()), this, SLOT(searchInDB()));
    connect(ui->goBackBtn, SIGNAL(clicked()), this, SLOT(goBackSlot()));
    connect(ui->shoeInMapBtn, SIGNAL(clicked()), this, SLOT(showInMap()));
    connect(ui->clearDataBtn, SIGNAL(clicked()), this, SLOT(clearDB()));
    connect(ui->setBtn, SIGNAL(clicked()), this, SLOT(setSlot()));
}

void mainWin::initCan()
{
    int row = 0;
    Sequence *temp = new Sequence();
    QModelIndex indextemp = ui->mainTable->model()->index(row,0);
    string id = indextemp.data().toString().toStdString();
    if (id != "")
    {
        db->getSequenceByID(tName,temp,id);
        ui->canvasWidget->setSequences(temp,1);
    }
}

void mainWin::search(Sequence input)
{
    this->input = input;
    refreshTable();
    searchMode(true);
    string tb = tName;
    QStringList *allTc = db->getAllTracID(tb);
    Sequence sf;

    double dfDis;
    for (int i = 0;i < allTc->length();i++)
    {
        db->getSequenceByID(tb,&sf,QString((*allTc)[i]).toStdString());
        dfDis = computeDiscreteFrechet(&input,&sf);

        QTableWidgetItem *tItem = new QTableWidgetItem();
        tItem->setData(Qt::DisplayRole,dfDis);
        ui->mainTable->setItem(i,2,tItem);
    }
    ui->mainTable->sortItems(2,Qt::AscendingOrder);
    delete allTc;
}

void mainWin::initMap()
{
     map = new MapWindow();
     map->editJs();
     map->draw();
}

void mainWin::searchMode(bool inSearch)
{
    search_mode = inSearch;
    if (inSearch)
    {
        ui->infoBox->hide();
        ui->searchBox->show();
    }
    else {
        ui->searchBox->hide();
        ui->infoBox->show();
    }
}

void mainWin::getDetail(Sequence *se_a)
{
    ui->idPtLabel->setText(se_a->getID());
    ui->startPtLabel->setText(QString::number(se_a->pts[0].longitude)
                              + " "
                              + QString::number(se_a->pts[0].latitude));
    ui->endPtLabel->setText(QString::number(se_a->pts[se_a->getNum() - 1].longitude)
                            + " "
                            + QString::number(se_a->pts[se_a->getNum() - 1].latitude));

    if (se_a->pts[0].time == "")
        ui->timePtLabel->setText(tr("No"));
    else {
        ui->timePtLabel->setText(tr("Yes"));
    }
}

void mainWin::openFile()
{
    QString file_name = QFileDialog::getOpenFileName(this,
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
    fprintf(stderr,"%s\n", fileName.c_str());
    string trcID;
    ifstream fin(fileName.c_str());

    Csv csv(fin);
    Sequence se;
    trcID = db->insertData(&csv, tName);
    db->getSequenceByID(tName,&se,trcID);
    se.outputTrac();

    refreshTable();

    QMessageBox *test = new QMessageBox(tr("Import Dialog"), tr("import finished"),QMessageBox::Information,
                                        QMessageBox::Ok|QMessageBox::Default,
                                        QMessageBox::No|QMessageBox::Escape,
                                        QMessageBox::NoButton);
    test->show();
}

void mainWin::showMap()
{
    map->deleteJS();
    map->initMap();
    map->show();
    map->showPoints(true);

    QStringList *allTascID = db->getAllTracID(tName);
    int num = allTascID->length();
    Sequence *all = new Sequence[num];
    string id;

    for (int i = 0;i<num;i++)
    {

        id = QString((*allTascID)[i]).toStdString();
        db->getSequenceByID(tName,&all[i],id);
        if (i == 0)
        {
            Point temp = all[i].getCentralPoint();
            map->setCentralPoint(temp.longitude,temp.latitude,10);
        }
        map->drawSqu(&(all[i]),i,3);
    }
    map->reload();

    this->setAttribute(Qt::WA_ShowWithoutActivating, true);
}

void mainWin::about()
{
    QMessageBox::aboutQt(this, "test");
}

void mainWin::searchInDB()
{
    QString file_name = QFileDialog::getOpenFileName(this,
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
    search(inputSe);
}

void mainWin::paint()
{
    can = new Canvas();
    QStringList *allTascID = db->getAllTracID(tName);
    int num = allTascID->length();
    Sequence *all = new Sequence[num];

    string id;
    for (int i = 0;i<num;i++)
    {
        id = QString((*allTascID)[i]).toStdString();
        db->getSequenceByID(tName,&all[i],id);
    }
    can->setSequences(all,num);
    can->show();
}

void mainWin::clearDB()
{
    db->clearDB(tName);
    refreshTable();
}

void mainWin::calActSlot()
{
    calWin->setWindowModality(Qt::ApplicationModal);
    calWin->show();
}

void mainWin::openFilesSlot()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                         tr("Open File"),
                                                         "",
                                                         "CSV Files(*.csv)",
                                                         0);
    if(fileNames.length() == 0)
    {
        return;
    }
    for (int i = 0; i < fileNames.length() ; i++)
    {
        string fileName = fileNames[i].toLocal8Bit().data();
        string trcID;
        ifstream fin(fileName.c_str());
        Csv csv(fin);
        trcID = db->insertData(&csv, tName);
//        if (db->isTableExist(tName))
//        {
//            trcID = db->insertData(&csv, tName);
//        }
//        else {
//            fprintf(stderr,"No table %s", tName.c_str());
//        }
    }
    refreshTable();
}

void mainWin::clickTBSlot(const QModelIndex index)
{
    int curRow = index.row();
    QModelIndex indextemp = ui->mainTable->model()->index(curRow,0);
    Sequence *temp = new Sequence();

    string id = indextemp.data().toString().toStdString();
    if (id != "")
    {
        db->getSequenceByID(tName,temp,id);
        if (search_mode)
        {
//            ui->canvasWidget->drawSeq(temp,Qt::blue, 1);
//            ui->canvasWidget->drawSeq(&input, Qt::red, 1);
            Sequence *p = new Sequence[2];
            p[0] = *temp;
            p[1] = input;
            ui->canvasWidget->setSequences(p,2);
        }
        else
        {
            ui->canvasWidget->setSequences(temp,1);
        }
        getDetail(temp);
    }
}

void mainWin::deleteOneAct()
{
    searchMode(false);
    selectedSeqS= getCurrentSeqs();
    if (selectedSeqS.length() == 0)
    {
        return;
    }
    Sequence *t = new Sequence();
    t = &(selectedSeqS[0]);
    //ui->canvasWidget->setSequences(t, 1);
    db->delSeq((t->getID()).toStdString(), tName);
    refreshTable();
}

void mainWin::searchSlot_R()
{
    selectedSeqS= getCurrentSeqs();
    if (selectedSeqS.length() == 0)
    {
        return;
    }
    Sequence t = selectedSeqS[0];
    search(t);
}

void mainWin::showInMapSlot_R()
{
    map->deleteJS();
    map->initMap();
    map->showPoints(true);
    map->showTimes(true);
    selectedSeqS= getCurrentSeqs();
    if (selectedSeqS.length() == 0)
    {
        return;
    }
    if (search_mode)
    {
        QVector<Sequence> se_draw;
        se_draw.append(input);
        se_draw.append(selectedSeqS[0]);
        Point center = getCenterPoint(se_draw);
        map->setCentralPoint(center,5);
        map->drawSequences(se_draw);
//        Sequence *p = new Sequence[2];
//        p[0] = selectedSeqS[0];
//        p[1] = input;
//        map->drawSequences(p,2);
    }
    else
    {
        Point center = getCenterPoint(selectedSeqS);
        map->setCentralPoint(center,5);
        map->drawSequences(selectedSeqS);
//        for (int i= 0;i<selectedSeqS.length(); i++)
//        {
//            map->drawSqu(&(selectedSeqS[i]), i, 3);
//        }
    }

    //map->setCentralPoint(selectedSeqS[0].getCentralPoint().longitude, selectedSeqS[0].getCentralPoint().latitude,8);
    map->reload();
    map->show();
}

void mainWin::detailSlot_R()
{
    selectedSeqS= getCurrentSeqs();
    if (selectedSeqS.length() == 0)
    {
        return;
    }
    detWin->showSeq(selectedSeqS[0]);
    detWin->show();
}

void mainWin::goBackSlot()
{
    if (search_mode)
    {
        searchMode(false);
    }
    refreshTable();
    return;
}

void mainWin::showInMap()
{
    map->deleteJS();
    map->initMap();
    selectedSeqS= getCurrentSeqs();
    if (selectedSeqS.length() == 0)
    {
        return;
    }
    map->setCentralPoint(selectedSeqS[0].getCentralPoint().longitude, selectedSeqS[0].getCentralPoint().latitude,8);
    map->drawSqu(&(selectedSeqS[0]), 1, 3);
    map->reload();
    map->show();
}

void mainWin::setSlot()
{
    setWin->show();
}

