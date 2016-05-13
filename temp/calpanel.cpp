#include "calpanel.h"
#include "ui_calpanel.h"


calPanel::calPanel(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::calPanel)
{
    setWin = new mapSettingWin();
    p = new Sequence();
    q = new Sequence();
    ui->setupUi(this);
    connect(ui->file1Btn, SIGNAL(clicked()), this, SLOT(openFile1()));
    connect(ui->file2Btn, SIGNAL(clicked()), this, SLOT(openFile2()));
    connect(ui->startBtn, SIGNAL(clicked()), this, SLOT(startSlot()));
    connect(ui->settingButton, SIGNAL(clicked()), this, SLOT(settingSlot()));
    ui->mapWidget->editJs();
    ui->mapWidget->initJS();
    ui->mapWidget->drawSqu(NULL);
    ui->mapWidget->draw();
    ui->mapWidget->reload();
}

calPanel::~calPanel()
{
    delete ui;
}

void calPanel::closeEvent(QCloseEvent* event)
{

    QMessageBox::StandardButton button;
        button = QMessageBox::question(this, tr("退出程序"),
            QString(tr("是否结束操作退出?")),
            QMessageBox::Yes | QMessageBox::No);

        if (button == QMessageBox::No) {
            event->ignore();  //忽略退出信号，程序继续运行
        }
        else if (button == QMessageBox::Yes) {
            ui->mapWidget->initJS();
            ui->mapWidget->drawSqu(NULL);
            ui->mapWidget->draw();
            ui->mapWidget->closeJS();
            ui->mapWidget->reload();
            event->accept();  //接受退出信号，程序退出
        }
}

void calPanel::showEvent(QShowEvent *event)
{
    ui->mapWidget->initJS();
    ui->mapWidget->drawSqu(NULL);
    ui->mapWidget->draw();

    qDebug() << "Show";
    event->accept();
}


void calPanel::openFile1()
{
    if (p->getNum() != 0)
    {
        Sequence *t = new Sequence();
        t = p;
        delete t;
        p = NULL;
        p = new Sequence();
    }
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
    ui->file1Path->setText(file_name);
    //string fileName = file_name.toStdString();
    string fileName = file_name.toLocal8Bit().data();
    ifstream fin(fileName.c_str());
    Csv csv(fin);
    getSquFromFile(&csv,p);
    fin.close();
}

void calPanel::openFile2()
{
    if (q->getNum() != 0)
    {
        Sequence *t = new Sequence();
        t = q;
        delete t;
        q = NULL;
        q = new Sequence();
    }
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
    ui->file2Path->setText(file_name);
    string fileName = file_name.toLocal8Bit().data();
    ifstream fin(fileName.c_str());
    Csv csv(fin);
    getSquFromFile(&csv,q);
    fin.close();
}

void calPanel::startSlot()
{
    ui->mapWidget->deleteJS();
    ui->mapWidget->initMap();
    ui->mapWidget->showPoints(true);
    if (p->getNum()*q->getNum() == 0)
    {
        return;
    }
    else
    {
        double x = (p->getCentralPoint().longitude + q->getCentralPoint().longitude)/2;
        double y = (p->getCentralPoint().latitude + q->getCentralPoint().latitude)/2;
        ui->mapWidget->setCentralPoint(x, y, 10);

        Sequence *test = new Sequence[2];
        test = longestCommonSeq(*p,*q,0.5);
        int ps = p->getIndex(test[0].pts[0]);
        int qs = q->getIndex(test[1].pts[0]);
        int pe = p->getIndex(test[0].pts[test[0].getNum() - 1]);

        int qe = q->getIndex(test[1].pts[test[1].getNum() - 1]);
        ui->mapWidget->drawSqu(p,1);
        ui->mapWidget->drawSqu(q,2);
        ui->mapWidget->highLightPart(p, pe, ps, 7,10);
        ui->mapWidget->highLightPart(q, qe, qs, 7,10);

//        QVector<SecCompare> temp = getBestSce(findSimilarSection(p, q));

//        ui->mapWidget->drawSqu(p,1);
//        ui->mapWidget->drawSqu(q,2);
//        if (temp.length() != 0)
//        {
//            for (int i = 0; i < temp.length(); i++)
//            {
//                ui->mapWidget->highLightPart(p, temp[i].beginIndex1, temp[i].endIndex1, 7, 10);
//                ui->mapWidget->highLightPart(q, temp[i].beginIndex2, temp[i].endIndex2, 7, 10);
//            }
//        }


        ui->mapWidget->reload();
    }
}

void calPanel::settingSlot()
{
    setWin->show();
}
