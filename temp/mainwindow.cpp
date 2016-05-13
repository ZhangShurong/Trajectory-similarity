#include "mainwindow.h"
#include "core.h"
mainWindow::mainWindow(QWidget *parent) : QWidget(parent)
{
    tName = "importtest";

    mainLayout = new QHBoxLayout();

    mainTable = new QTableWidget();

    menuBar = new QMenuBar();

    createActions();
    createMenus();
    initLeft();
    initTable();

    mainLayout->setMenuBar(menuBar);
    mainLayout->addWidget(left);
    mainLayout->addWidget(mainTable);

    setLayout(mainLayout);
    initCSS();
}

void mainWindow::setDb(DataBase *database)
{
    db = database;
}

void mainWindow::loadFile()
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
}


void mainWindow::initCSS()
{
    cssFile = new QFile(":/css/mainWin.qss");
    cssFile->open(QIODevice::ReadOnly);
    strCSS = new QString(QLatin1String(cssFile->readAll()));
    this->setStyleSheet(*strCSS);
    cssFile->close();
}

void mainWindow::initLeft()
{
    left = new QWidget(this);
    leftLayout = new QVBoxLayout(left);
    openFileButton = new QPushButton(tr("Open File"));
    calButton = new QPushButton(tr("Calculate"));

    leftLayout->addWidget(openFileButton);
    leftLayout->addWidget(calButton);
    left->setLayout(leftLayout);
}
void mainWindow::createActions()
{
    //fileMenu
    openAct = new QAction(tr("&Import"), this);
    openAct->setShortcuts(QKeySequence::New);
    openAct->setStatusTip(tr("Import tracs from csv file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));
    importFiles = new QAction(tr("&Import Files"), this);
    connect(importFiles,SIGNAL(triggered()), this, SLOT(openFilesSlot()));
    exportOne = new QAction(tr("&Export one sequence"),this);
    exportAll = new QAction(tr("&Export all sequences"),this);

    showMapAct = new QAction(tr("&ShowMap"), this);
    showMapAct->setStatusTip(tr("Show Ttacs in a map"));
    connect(showMapAct, SIGNAL(triggered()), this, SLOT(showMap()));

    searchAct = new QAction(tr("&Search"), this);
    searchAct->setStatusTip(tr("Search in database"));
    connect(searchAct, SIGNAL(triggered()), this, SLOT(searchInDB()));

    paintAct = new QAction(tr("&Paint"),this);
    connect(paintAct,SIGNAL(triggered()),this,SLOT(paint()));

    //dbMenu
    clearAct = new QAction(tr("&Clear Database"),this);
    connect(clearAct, SIGNAL(triggered()), this, SLOT(clearDB()));
    showAct = new QAction(tr("&Show Database"),this);

    //calMenu
    calAct = new QAction(tr("&Calculate"),this);
    connect(calAct, SIGNAL(triggered()), this, SLOT(calActSlot()));
}

void mainWindow::createMenus()
{
    fileMenu = new QMenu();
    fileMenu->setTitle(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(importFiles);
    fileMenu->addAction(exportOne);
    fileMenu->addAction(exportAll);

    mapMenu = new QMenu();
    mapMenu->setTitle(tr("&Map"));
    mapMenu->addAction(showMapAct);
    mapMenu->addAction(paintAct);

    tracMenu = new QMenu();
    tracMenu->setTitle(tr("&Trac"));
    tracMenu->addAction(searchAct);

    dbMenu = new QMenu();
    dbMenu->setTitle(tr("&DataBase"));
    dbMenu->addAction(clearAct);
    dbMenu->addAction(showAct);

    calMenu = new QMenu();
    calMenu->setTitle(tr("&Similarity calculation"));
    calMenu->addAction(calAct);

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(mapMenu);
    menuBar->addMenu(tracMenu);
    menuBar->addMenu(dbMenu);
    menuBar->addMenu(calMenu);
}

void mainWindow::initTable()
{
    mainTable->setColumnCount(4);
    mainTable->setRowCount(50);

    mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList header;
    header << tr("ID")
           << tr("Dementions")
           << tr("PointNumber")
           << tr("test");
    mainTable->setHorizontalHeaderLabels(header);
    refreshTable();

}

void mainWindow::refreshTable()
{
    QStringList *tracs;
    tracs = db->getAllTracID(tName);
    if (tracs->length() == 0)
        mainTable->clear();
    else {
        for(int i = 0; i < tracs->length();i++)
        {
            QString temp = (*tracs)[i];
            mainTable->setItem(i,0, new QTableWidgetItem(temp));
            mainTable->setItem(i,2,
                               new QTableWidgetItem(QString::number(db->getPointNumByID(temp.toStdString()))));
            //qDebug()<<(*tracs)[i]<<endl;
        }
    }
}

void mainWindow::openFile()
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

    std::locale::global(std::locale(""));
    //file_name.replace("\\","/");
    //string fileName = file_name.toStdString();
    string fileName = file_name.toLocal8Bit().data();
    fprintf(stderr,"%s\n", fileName.c_str());
    string trcID;
    ifstream fin(fileName.c_str());


    Csv csv(fin);
    Sequence se;

    if (db->isTableExist(tName))
    {
        trcID = db->insertData(&csv, tName);
        db->getSequenceByID(tName,&se,trcID);
        se.outputTrac();
    }
    else {

        fprintf(stderr,"No table %s", tName.c_str());
    }
    fprintf(stdout, "The id is %s", trcID.c_str());
    refreshTable();

    QMessageBox *test = new QMessageBox(tr("Import Dialog"), tr("import finished"),QMessageBox::Information,
                                        QMessageBox::Ok|QMessageBox::Default,
                                        QMessageBox::No|QMessageBox::Escape,
                                        QMessageBox::NoButton);
    test->show();
}

void mainWindow::showMap()
{
    map = new MapWindow();
    map->show();
    map->editJs();
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
    map->draw();
    this->setAttribute(Qt::WA_ShowWithoutActivating, true);

}

void mainWindow::about()
{
    QMessageBox::aboutQt(this, "test");
}

void mainWindow::searchInDB()
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

    //string fileName = file_name.toStdString();
    string fileName = file_name.toLocal8Bit().data();
    ifstream fin(fileName.c_str());
    Csv csv(fin);
    Sequence inputSe;
    getSquFromFile(&csv,&inputSe);

    string tb = "importtest";
    QStringList *allTc = db->getAllTracID(tb);
    Sequence sf;
    double dfDis;

    for (int i = 0;i < allTc->length();i++)
    {
        db->getSequenceByID(tb,&sf,QString((*allTc)[i]).toStdString());
        dfDis = computeDiscreteFrechet(&inputSe,&sf);
        fprintf(stderr,"%f\n",dfDis);

        QString temp = (*allTc)[i];
        mainTable->setItem(i,0, new QTableWidgetItem(temp));
        mainTable->setItem(i,2,
                           new QTableWidgetItem(QString::number(db->getPointNumByID(temp.toStdString()))));

        QTableWidgetItem *tItem = new QTableWidgetItem();
        tItem->setData(Qt::DisplayRole,dfDis);
        mainTable->setItem(i,3,tItem);
    }
    mainTable->sortItems(3,Qt::AscendingOrder);
}

void mainWindow::paint()
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

void mainWindow::clearDB()
{
    db->clearDB(tName);
    refreshTable();
}

void mainWindow::calActSlot()
{
    calWin = new calWindow();
    calWin->show();
}

void mainWindow::openFilesSlot()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                         tr("Open File"),
                                                         "",
                                                         "CSV Files(*.csv)",
                                                         0);
    qDebug() << fileNames[0];
}

