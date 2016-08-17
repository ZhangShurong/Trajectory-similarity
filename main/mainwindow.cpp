#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QProgressDialog>
#include "database.h"
#include <QtCore/QCoreApplication>
#include <QtSql>
#include "core.h"
#include "omp.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    tName("importtest")
   // importThread(db, tName, QStringList()),
    //importProgressDialog(NULL)
{
    db = new DataBase();
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->startAnimation();

    tracs = new QStringList();
    detWin = new detailWin();
    cal = new CalWindow(ui);
    searchWin = new SearchWin(ui, db);
    settingWin = new SettingWin(ui);
    cloudWin = new Cloud(ui);

    ui->mapWidget->setHtml("./html/cal.html");
    ui->mapWidget->setJs("./html/cal.js");
    ui->mapWin->setHtml("./html/map.html");
    ui->mapWin->setJs("./html/map.js");
    ui->searchMap->setHtml("./html/search.html");
    ui->searchMap->setJs("./html/search.js");


    showedInMap=false;

    initMap();
    initAction();

    // initCSS();
    initSig();

    initCan();
    initTable();

    searchMode(false);
}

void MainWindow::setDb(DataBase *database)
{
    db = database;
    searchWin->setDB(database);
}

QVector<Sequence> MainWindow::getCurrentSeqs()
{
    QVector<Sequence> tempSeq;
    QModelIndexList indexes = ui->mainTable->selectionModel()->selectedIndexes();
    if (indexes.length() != 0) {
        for (int i = 0; i < indexes.length(); i=i+4) {
            Sequence *temp = new Sequence();
            int curRow = indexes[i].row();
            QModelIndex indextemp = ui->mainTable->model()->index(curRow,0);
            string id = indextemp.data().toString().toStdString();
            if (id != "") {
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
    if ( event->button() == Qt::LeftButton ) {
        startPos = event->globalPos() - this->frameGeometry().topLeft();
    } else if ( event->button() == Qt::MiddleButton)
        closeWindowAnimation();
    else if(  event->button() == Qt::RightButton )
        shakeWindow();
}
/*
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{

    if ( event->buttons() == Qt::LeftButton )
        {
            endPos = event->globalPos() - startPos;
            this->move(endPos);
    }

}
*/


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
    animation->setDuration(ANIMATE_DURATION);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    //connect(animation,SIGNAL(QPropertyAnimation::finished()),this, SLOT(close()));
    connect(animation,SIGNAL(finished()),this, SLOT(close()));

}

void MainWindow::startAnimation()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this,"windowOpacity");
    animation->setDuration(ANIMATE_DURATION);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
}

void MainWindow::on_searchToolButton_clicked()
{
    searchWin->clearInput();
    refreshTable();
    if (ui->stackedWidget->currentIndex() !=1 ) {
        initSearchWin();
    }
    this->setStackCurrentPage(1);

}

void MainWindow::on_checkToolButton_2_clicked()
{
    if(!showedInMap){
       refreshTable();
    }
    searchMode(false);
    this->setStackCurrentPage(0);
}

void MainWindow::on_calToolButton_clicked()
{
    if (ui->stackedWidget->currentIndex() !=2 ) {
        this->ui->mapWidget->initJS();
        this->ui->mapWidget->setDefaultCentralPt();
        this->ui->mapWidget->reload();
    }
    this->setStackCurrentPage(2);
}

void MainWindow::on_mapToolButton_clicked()
{
    if (ui->stackedWidget->currentIndex() !=3 ) {
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
    this->setStackCurrentPage(6);
}

void MainWindow::aboutButton_clicked()
{
    this->setStackCurrentPage(5);
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
    if (!file_name.isNull()) {
        qDebug()<<file_name;
    } else {
        return;
    }
    string fileName = file_name.toLocal8Bit().data();


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

    for (int i = 0; i<num; i++) {
        id = QString((*tracs)[i]).toStdString();
        db->getSequenceByID(tName,&temp,id);
        if (i == 0) {
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
    QMessageBox::information(NULL, "提示", "本页面为简易搜索，获得详细信息请转到搜素界面", QMessageBox::Yes, QMessageBox::Yes);
    QString file_name = QFileDialog::getOpenFileName(this,
                        tr("Open File"),
                        "",
                        "CSV Files(*.csv)",
                        0);
    if (!file_name.isNull()) {
        qDebug()<<file_name;
    } else {
        return;
    }
    string fileName = file_name.toLocal8Bit().data();
    ifstream fin(fileName.c_str());
    Csv csv(fin);
    Sequence inputSe;
    getSquFromFile(&csv,&inputSe);
    if (inputSe.getNum() == 0) {
        QMessageBox::information(NULL, "Error", "格式错误", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
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
    for (int i = 0; i<num; i++) {
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

    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open File"),
                            "", "CSV Files(*.csv)", 0);
    if (fileNames.isEmpty()) {
        return;
    }
    int n=fileNames.length();
    QProgressDialog importProgressDialog(tr("正在导入轨迹段数据，请稍候..."),
                         tr("取消"),
                         0, n);
    importProgressDialog.setWindowModality(Qt::WindowModal);
    importProgressDialog.setWindowTitle(tr("Please Wait"));   //设置标题的显示时间
    importProgressDialog.show();

    int c=0;
   // importProgressDialog.setValue(1);
    db->db.transaction();
    string fileName;
//    ifstream fin2;
//    Csv format;

    for (int i = 0; i < fileNames.length(); i++)
        {
            fileName = fileNames[i].toLocal8Bit().data();
            ifstream fin2(fileName.c_str());
            Csv format(fin2);

           try{
             db->insertData(&format, tName);
             c++;
            }catch(int j){
             importFileErrorSlot(j);
             break;
            }
            if (importProgressDialog.wasCanceled()) {
                break;
            }
            importProgressDialog.setValue(i);
        }
        db->db.commit();
        refreshTable();
        importProgressDialog.setValue(fileNames.length());
        importFinished(c,fileNames.length()-c);
}

void MainWindow::clickTBSlot(const QModelIndex index)
{
    int curRow = index.row();
    QModelIndex indextemp = ui->mainTable->model()->index(curRow,0);
    Sequence *temp = new Sequence();

    string id = indextemp.data().toString().toStdString();
    if (id != "") {
        db->getSequenceByID(tName,temp,id);
        if (search_mode) {
            Sequence *p = new Sequence[2];
            p[0] = *temp;
            p[1] = input;
            ui->canvasWidget->setSequences(p,2);

        } else {
            ui->canvasWidget->setSequences(temp,1);
        }
        getDetail(temp);
    }
}

void MainWindow::deleteOneAct()
{
    searchMode(false);
    selectedSeqS= getCurrentSeqs();
    if (selectedSeqS.length() == 0) {
        return;
    }
    Sequence *t = new Sequence();
    t = &(selectedSeqS[0]);
    db->delSeq((t->getID()).toStdString(), tName);
    refreshTable();
}

void MainWindow::searchSlot_R()
{
    progress = new QProgressDialog(tr("正在计算轨迹数据，请稍候..."), tr("取消"),
                                                       0,tracs->length(),this);

    progress->setWindowModality(Qt::WindowModal);
    progress->setWindowTitle(tr("Please Wait"));
   // progress->setLabelText(tr("processing , please wait..."));
    progress->show();
    qApp->processEvents();
    selectedSeqS= getCurrentSeqs();
    if (selectedSeqS.length() == 0) {
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
    ui->mapWin->showEndPoints(true);
    selectedSeqS= getCurrentSeqs();
    if (selectedSeqS.length() == 0) {
        return;
    }
    if (search_mode) {
        QVector<Sequence> se_draw;
        se_draw.append(input);
        se_draw.append(selectedSeqS[0]);
        Point center = getCenterPoint(se_draw);
        ui->mapWin->setCentralPoint(center,getZoom(se_draw));
        ui->mapWin->drawSequences(se_draw);
    } else {
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
    if (selectedSeqS.length() == 0) {
        return;
    }
    detWin->setWindowModality(Qt::ApplicationModal);
    detWin->showSeq(selectedSeqS[0]);
    detWin->show();
}

void MainWindow::goBackSlot()
{
    if (search_mode) {
        searchMode(false);
    }
    refreshTable();
    return;
}

void MainWindow::showInMap()
{
    showedInMap=true;
    showInMapSlot_R();
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
    if(filename.isEmpty()) {
        return;
    } else {
        qDebug() << filename;
        filename += ".csv";
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "Failed!";
            return;
        }
        QTextStream out(&file);
        selectedSeqS= getCurrentSeqs();
        if (selectedSeqS.length() == 0) {
            qDebug() << "Nothing selected!";
            return;
        } else {
            for (int i = 0; i< selectedSeqS.length(); i++) {
                for (int j = 0; j < selectedSeqS[i].getNum(); j++) {
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
    deleteAct = new QAction(tr("删除"), this);
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteOneAct()));
    searchAct = new QAction(tr("搜索"),this);
    connect(searchAct, SIGNAL(triggered()), this, SLOT(searchSlot_R()));
    showInMapAct = new QAction(tr("在地图中显示"),this);
    connect(showInMapAct, SIGNAL(triggered()), this, SLOT(showInMapSlot_R()));
    detailAct = new QAction(tr("详情"),this);
    connect(detailAct, SIGNAL(triggered()), this, SLOT(detailSlot_R()));
//    connect(&importThread, SIGNAL(importedOneFileSignal(int)), this, SLOT(refreshTable()));
//    connect(&importThread, SIGNAL(importFinishedSignal(int,int)), this, SLOT(importFinished(int,int)));
//    connect(&importThread, SIGNAL(importFileErrorSignal(int)), this,SLOT(importFileErrorSlot(int)));
//    connect(&importThread, SIGNAL(importedOneFileSignal(int)), this, SLOT(refreshValue(int)));
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
        if (tracs->length() > ROW_NUM) {
            ui->mainTable->setRowCount(tracs->length() + 10);
        } else {
            ui->mainTable->setRowCount(ROW_NUM + 10);
        }
        for(int i = 0; i < tracs->length(); i++) {
            QString temp = (*tracs)[i];
            ui->mainTable->setItem(i,0, new QTableWidgetItem(temp));
            ui->mainTable->setItem(i,1,
                                   new QTableWidgetItem(QString::number(db->getPointNumByID(temp.toStdString()))));
        }
    }
    searchWin->setTracs(tracs);

    // delete tracs;
}

//void MainWindow::refreshValue(int n){

//       //importProgressDialog->setValue(n);
////       if ( importProgressDialog->wasCanceled()) {
////                   break;
////           }
//}

void MainWindow::importFileErrorSlot(int code)
{
    QMessageBox::information(NULL, tr("导入文件出现错误！ 代码") + QString::number(code),
                             "时间格式错误,格式类似 20160601 13:00:11");
}

void MainWindow::importFinished(int ok, int bad)
{
    QMessageBox::information(this, tr("导入文件结束"),
                             QString::number(ok) + tr(" 个文件导入成功，\n") +
                             QString::number(bad) + tr(" 个文件导入失败。"));
    //importProgressDialog->hide();
}

void MainWindow::cloudBtnClicked()
{
    this->setStackCurrentPage(9);
}

void MainWindow::clusterBtnClicked()
{
    this->setStackCurrentPage(8);
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
    connect(ui->cloudBtn, SIGNAL(clicked(bool)), this, SLOT(cloudBtnClicked()));
    connect(ui->clusterBtn, SIGNAL(clicked(bool)), this, SLOT(clusterBtnClicked()));

}

void MainWindow::initCan()
{
    int row = 0;
    Sequence *temp = new Sequence();
    QModelIndex indextemp = ui->mainTable->model()->index(row,0);
    string id = indextemp.data().toString().toStdString();
    if (id != "") {
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
    if(input.getNum() == 0) {
        return;
    }
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

    QTableWidgetItem *tItem;
    Core core;
    for (int i = 0; i < tracs->length(); i++) {
        progress->setValue(i);
        if (progress->wasCanceled()) {
            break;
        }
        tItem = new QTableWidgetItem();
        db->getSequenceByID(tb,&sf,QString((*tracs)[i]).toStdString());
        dfDis = core.computeDiscreteFrechet(&input,&sf);

        //qc = getSimplify(&input,&sf,beginMin1,beginMin2);

        tItem->setData(Qt::DisplayRole,dfDis);
        ui->mainTable->setItem(i,2,tItem);
        if (dfDis >= maxDis)
            maxDis = dfDis;

    }
    //填充百分数
    for (int i = 0; i < tracs->length(); i++) {

        QString temp = ui->mainTable->item(i,2)->text();
        dfDis = temp.toDouble();
        double percent = ((maxDis - dfDis)/maxDis)*100;

        QTableWidgetItem *tItem = new QTableWidgetItem();
        if (percent < 0) {
            tItem->setData(Qt::DisplayRole,"0%");
        } else {
            tItem->setData(Qt::DisplayRole,
                           QString::number(percent) + "%");
        }

        ui->mainTable->setItem(i,3,tItem);    
    }

    ui->mainTable->sortItems(2,Qt::AscendingOrder);
    progress->setValue(tracs->length());
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
    if (inSearch) {
        ui->modeLabel->setText("进入搜索模式，点击返回回到正常模式");
        //ui->stackedWidget_2->setCurrentIndex(1);
    } else {
        ui->modeLabel->setText("正常模式");
        ui->stackedWidget_2->setCurrentIndex(0);
    }
}

void MainWindow::getDetail(Sequence *se_a)
{
    if(se_a->getNum() == 0) {
        ui->idPtLabel->setText(se_a->getID());
        ui->startPtLabel->setText("-1 -1");
        ui->endPtLabel->setText("-1 -1 ");
        ui->timePtLabel->setText(tr("No"));
    }
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

