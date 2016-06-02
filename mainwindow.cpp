#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QProgressDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->startAnimation();

    tName = "importtest";
    tracs = new QStringList();
    detWin = new detailWin();
    cal = new CalWindow(ui);
    searchWin = new SearchWin(ui, db);
    settingWin = new SettingWin(ui);
    initMap();
    initAction();
    initTable();
   // initCSS();
    initSig();
    initCan();



//    searchWin->setDB(db);
    searchMode(false);
}

void MainWindow::setDb(DataBase *database)
{
    db = database;
}

QVector<Sequence> MainWindow::getCurrentSeqs()
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
                tempSeq.append(*temp);
            }
        }
    }
    return tempSeq;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_closeToolButton_clicked()
{
    this->closeWindowAnimation();
}

void MainWindow::on_minToolButton_clicked()
{
    this->showMinimized();
}

/*窗口拖动*/
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton )
        {
            startPos = event->globalPos() - this->frameGeometry().topLeft();
        }
        else if ( event->button() == Qt::MiddleButton)
            closeWindowAnimation();
        else if(  event->button() == Qt::RightButton )
        shakeWindow();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if ( event->buttons() == Qt::LeftButton )
        {
            endPos = event->globalPos() - startPos;
            this->move(endPos);
    }
}


void MainWindow::shakeWindow()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this,"geometry");
    animation->setDuration(500);
    animation->setKeyValueAt(0,QRect(QPoint(this->frameGeometry().x()-3,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.1,QRect(QPoint(this->frameGeometry().x()+6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.2,QRect(QPoint(this->frameGeometry().x()-6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.3,QRect(QPoint(this->frameGeometry().x()+6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.4,QRect(QPoint(this->frameGeometry().x()-6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.5,QRect(QPoint(this->frameGeometry().x()+6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.6,QRect(QPoint(this->frameGeometry().x()-6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.7,QRect(QPoint(this->frameGeometry().x()+6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.8,QRect(QPoint(this->frameGeometry().x()-6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(0.9,QRect(QPoint(this->frameGeometry().x()+6,this->frameGeometry().y()),this->size()));
    animation->setKeyValueAt(1,QRect(QPoint(this->frameGeometry().x()-3,this->frameGeometry().y()),this->size()));
    animation->start();
}

void MainWindow::setStackCurrentPage(quint8 index)
{
    QWidget *widget = ui->stackedWidget->widget(index);
    ui->stackedWidget->setCurrentWidget(widget);
}

void MainWindow::closeWindowAnimation()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this,"windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    //connect(animation,SIGNAL(QPropertyAnimation::finished()),this, SLOT(close()));
    connect(animation,SIGNAL(finished()),this, SLOT(close()));

}

void MainWindow::startAnimation()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this,"windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
}

void MainWindow::on_searchToolButton_clicked()
{
    refreshTable();
    if (ui->stackedWidget->currentIndex() !=1 )
    {
        initSearchWin();
    }
    this->setStackCurrentPage(1);

}

void MainWindow::on_checkToolButton_2_clicked()
{
    refreshTable();
    searchMode(false);
    this->setStackCurrentPage(0);
}

void MainWindow::on_calToolButton_clicked()
{
    if (ui->stackedWidget->currentIndex() !=2 )
    {
        this->ui->mapWidget->initJS();
        this->ui->mapWidget->setDefaultCentralPt();
        this->ui->mapWidget->reload();
    }
    this->setStackCurrentPage(2);
}

void MainWindow::on_mapToolButton_clicked()
{
    if (ui->stackedWidget->currentIndex() !=3 )
    {
        this->ui->mapWin->initJS();
        this->ui->mapWin->setDefaultCentralPt();
        this->ui->mapWin->reload();
    }
    this->setStackCurrentPage(3);
}

void MainWindow::on_opToolButton_clicked()
{
    this->setStackCurrentPage(4);
}

void MainWindow::on_exToolButton_clicked()
{
    this->setStackCurrentPage(5);
}

void MainWindow::aboutButton_clicked()
{
    this->setStackCurrentPage(6);
}

void MainWindow::on_gjToolButton_clicked()
{
    this->setStackCurrentPage(7);
}

void MainWindow::openFile()
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

void MainWindow::showMap()
{
    ui->mapWin->initJS();
    ui->mapWin->showPoints(true);
    ui->stackedWidget->setCurrentIndex(3);

    //QStringList *allTascID = db->getAllTracID(tName);
   // int num = allTascID->length();
     int num = tracs->length();
    Sequence temp;
    QVector<Sequence> all;
    string id;

    for (int i = 0;i<num;i++)
    {
        id = QString((*tracs)[i]).toStdString();
        db->getSequenceByID(tName,&temp,id);
        if (i == 0)
        {
            Point t = temp.getCentralPoint();
            ui->mapWin->setCentralPoint(t.longitude,t.latitude,10);
        }
        all.append(temp);
    }
    ui->mapWin->drawSequences(all);
    //ui->mapWin->drawSqu(all,i,3);
    ui->mapWin->reload();
}

void MainWindow::about()
{
    QMessageBox::aboutQt(this, "test");
}

void MainWindow::searchInDB()
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

void MainWindow::paint()
{
    can = new Canvas();
   // QStringList *allTascID = db->getAllTracID(tName);
    //int num = allTascID->length();
    int num = tracs->length();
    Sequence *all = new Sequence[num];

    string id;
    for (int i = 0;i<num;i++)
    {
        id = QString((*tracs)[i]).toStdString();
        db->getSequenceByID(tName,&all[i],id);
    }
    can->setSequences(all,num);
    can->show();
}

void MainWindow::clearDB()
{
    db->clearDB(tName);
    refreshTable();
}


void MainWindow::openFilesSlot()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                         tr("Open File"),
                                                         "",
                                                         "CSV Files(*.csv)",
                                                         0);
    if (fileNames.length() == 0)
    {
        return;
    }
    QProgressDialog progress(tr("正在导入轨迹数据，请稍候..."),
                             tr("取消"),
                             0, fileNames.length(), // Range
                             this);
    progress.setWindowModality(Qt::WindowModal);

    for (int i = 0; i < fileNames.length() ; i++)
    {
        progress.setValue(i);
        string fileName = fileNames[i].toLocal8Bit().data();
        string trcID;
        ifstream fin(fileName.c_str());
        Csv csv(fin);
        trcID = db->insertData(&csv, tName);
        if (progress.wasCanceled()) {
            break;
        }
    }
    progress.setValue(fileNames.length());
    refreshTable();
}

void MainWindow::clickTBSlot(const QModelIndex index)
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
            Sequence *p = new Sequence[2];
            p[0] = *temp;
            p[1] = input;
            ui->canvasWidget->setSequences(p,2);

            /*
             * 效率不够
            QVector<QString>qst;
            int beginMin1,beginMin2;
            QVector<SecCompare> qs1=findBest(temp,&input,beginMin1,beginMin2);

            if (qs1.length() != 0)
             {
                 for (int i = 0; i < qs1.length(); i++)
                 {
                     QString s =   QString::number(qs1[i].beginIndex1)+" "
                                 + QString::number(qs1[i].endIndex1)+" "
                                 + QString::number(qs1[i].beginIndex2)+" "
                                 + QString::number(qs1[i].endIndex2);
                    if(!qst.contains(s))
                      qst.append(s);
                 }
             }
             */
        }
        else
        {
            ui->canvasWidget->setSequences(temp,1);
        }
        getDetail(temp);
    }
}

void MainWindow::deleteOneAct()
{
    searchMode(false);
    selectedSeqS= getCurrentSeqs();
    if (selectedSeqS.length() == 0)
    {
        return;
    }
    Sequence *t = new Sequence();
    t = &(selectedSeqS[0]);
    db->delSeq((t->getID()).toStdString(), tName);
    refreshTable();
}

void MainWindow::searchSlot_R()
{
    selectedSeqS= getCurrentSeqs();
    if (selectedSeqS.length() == 0)
    {
        return;
    }
    Sequence t = selectedSeqS[0];
    search(t);
}

void MainWindow::showInMapSlot_R()
{
    ui->mapWin->initJS();
    ui->mapWin->setDefaultCentralPt();
    ui->mapWin->showPoints(true);
    ui->mapWin->showTimes(true);
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
        ui->mapWin->setCentralPoint(center,getZoom(se_draw));
        ui->mapWin->drawSequences(se_draw);
    }
    else
    {
        Point center = getCenterPoint(selectedSeqS);
        ui->mapWin->setCentralPoint(center,getZoom(selectedSeqS));
        ui->mapWin->drawSequences(selectedSeqS);
    }

    ui->mapWin->reload();
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::detailSlot_R()
{
    selectedSeqS= getCurrentSeqs();
    if (selectedSeqS.length() == 0)
    {
        return;
    }
    detWin->showSeq(selectedSeqS[0]);
    detWin->show();
}

void MainWindow::goBackSlot()
{
    if (search_mode)
    {
        searchMode(false);
    }
    refreshTable();
    return;
}

void MainWindow::showInMap()
{
    showInMapSlot_R();
    /*
    ui->mapWin->initJS();
    selectedSeqS= getCurrentSeqs();
    if (selectedSeqS.length() == 0)
    {
        return;
    }
    ui->mapWin->setCentralPoint(selectedSeqS[0].getCentralPoint().longitude, selectedSeqS[0].getCentralPoint().latitude,8);
    //ui->mapWin->drawSqu(&(selectedSeqS[0]), 1, 3);
    ui->mapWin->drawSequence(selectedSeqS[0],1,3);
    ui->mapWin->reload();
    //ui->mapWin->show();
    ui->stackedWidget->setCurrentIndex(3);
    */
}

void MainWindow::setSlot()
{
    //setWin->show();
}

void MainWindow::exportSlot()
{
    qDebug() << "exportBtn clicked";
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Export Sequence"),
        "",
        tr("*.csv;")); //选择路径
    if(filename.isEmpty())
    {
        return;
    }
    else
    {
        qDebug() << filename;
        filename += ".csv";
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "Failed!";
            return;
        }
        QTextStream out(&file);
        selectedSeqS= getCurrentSeqs();
        if (selectedSeqS.length() == 0)
        {
            qDebug() << "Nothing selected!";
            return;
        }
        else {
            for (int i = 0; i< selectedSeqS.length(); i++)
            {
                for (int j = 0; j < selectedSeqS[i].getNum(); j++)
                {
                    out << selectedSeqS[i].pts[j].longitude
                        << ","
                        << selectedSeqS[i].pts[j].latitude
                        << "\n";
                    out.flush();
                }
            }
        }
    }
}

void MainWindow::initAction()
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

void MainWindow::initCSS()
{
    cssFile = new QFile(":/css/mainWin.qss");
    cssFile->open(QIODevice::ReadOnly);
    strCSS = new QString(QLatin1String(cssFile->readAll()));
    this->setStyleSheet(*strCSS);
    cssFile->close();
}

void MainWindow::initTable()
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
    ui->mainTable->setRowCount(ROW_NUM);
    ui->mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    refreshTable();
}

void MainWindow::refreshTable()
{
    ui->mainTable->clearContents();
    QStringList header;
    header << "轨迹ID"
           << "轨迹点数"
           << "Frechet Distance"
           << "相似度";
    ui->mainTable->setHorizontalHeaderLabels(header);
    ui->mainTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tracs = db->getAllTracID(tName);
    if (tracs->length() == 0)
        ui->mainTable->clear();
    else {
        if (tracs->length() > ROW_NUM)
        {
            ui->mainTable->setRowCount(tracs->length() + 10);
        }
        else
        {
            ui->mainTable->setRowCount(ROW_NUM + 10);
        }
        for(int i = 0; i < tracs->length();i++)
        {
            QString temp = (*tracs)[i];
            ui->mainTable->setItem(i,0, new QTableWidgetItem(temp));
            ui->mainTable->setItem(i,1,
                               new QTableWidgetItem(QString::number(db->getPointNumByID(temp.toStdString()))));
        }
    }
    searchWin->setTracs(tracs);
   // delete tracs;
}

void MainWindow::initSig()
{
    connect(ui->aboutBtn, SIGNAL(clicked()), this, SLOT(aboutButton_clicked()));
    connect(ui->mainTable, SIGNAL(clicked(QModelIndex)), this, SLOT(clickTBSlot(QModelIndex)));
    connect(ui->importDataBtn, SIGNAL(clicked()), this, SLOT(openFilesSlot()));
    connect(ui->searchBtn, SIGNAL(clicked()), this, SLOT(searchInDB()));
    connect(ui->gobackBtn, SIGNAL(clicked()), this, SLOT(goBackSlot()));
    connect(ui->shoeInMapBtn, SIGNAL(clicked()), this, SLOT(showInMap()));
    //connect(ui->shoeInMapBtn, SIGNAL(clicked()), this, SLOT(showInMapSlot_R()));
    connect(ui->clearDataBtn, SIGNAL(clicked()), this, SLOT(clearDB()));
    connect(ui->exportBtn, SIGNAL(clicked()), this, SLOT(exportSlot()));
    connect(ui->detailBtn, SIGNAL(clicked()), this, SLOT(detailSlot_R()));
    connect(ui->deleteBtn, SIGNAL(clicked()), this, SLOT(deleteOneAct()));
    connect(ui->searchToolButton, SIGNAL(clicked()), this, SLOT(on_searchToolButton_clicked()));
    connect(ui->calToolButton, SIGNAL(clicked()), this, SLOT(on_calToolButton_clicked()));
}

void MainWindow::initCan()
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

void MainWindow::initSearchWin()
{
    this->ui->searchMap->initJS();
    this->ui->searchMap->setDefaultCentralPt();
    this->ui->searchMap->reload();
    searchWin->refreshTable();
    this->ui->searchPathEdit->clear();
    searchWin->init();
}

void MainWindow::search(Sequence input)
{
    this->input = input;
    refreshTable();
    searchMode(true);
    string tb = tName;
    Sequence sf;
    double dfDis;
    double maxDis = 0;

    /*----------
    QVector<QVector<int> >qc;
    int beginMin1=0;
    int beginMin2=0;
    */

    for (int i = 0;i < tracs->length();i++)
    {
        QTableWidgetItem *tItem = new QTableWidgetItem();
        db->getSequenceByID(tb,&sf,QString((*tracs)[i]).toStdString());
        dfDis = computeDiscreteFrechet(&input,&sf);

        //qc = getSimplify(&input,&sf,beginMin1,beginMin2);

        tItem->setData(Qt::DisplayRole,dfDis);
        ui->mainTable->setItem(i,2,tItem);
        if (dfDis >= maxDis)
            maxDis = dfDis;
    }
    //填充百分数
    for (int i = 0;i < tracs->length();i++)
    {
        QString temp = ui->mainTable->item(i,2)->text();
        dfDis = temp.toDouble();
        double percent = ((maxDis - dfDis)/maxDis)*100;

        QTableWidgetItem *tItem = new QTableWidgetItem();
        if (percent < 0)
        {
            tItem->setData(Qt::DisplayRole,"0%");
        }
        else
        {
            tItem->setData(Qt::DisplayRole,
                           QString::number(percent) + "%");
        }

        ui->mainTable->setItem(i,3,tItem);
    }

    ui->mainTable->sortItems(2,Qt::AscendingOrder);
}

void MainWindow::initMap()
{
    ui->mapWin->initJS();
    ui->mapWin->setDefaultCentralPt();
    ui->mapWin->reload();
}

void MainWindow::searchMode(bool inSearch)
{
    search_mode = inSearch;
    if (inSearch)
    {
        //ui->stackedWidget_2->setCurrentIndex(1);
    }
    else {
        ui->stackedWidget_2->setCurrentIndex(0);
    }
}

void MainWindow::getDetail(Sequence *se_a)
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

