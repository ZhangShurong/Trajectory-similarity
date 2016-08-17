#include "calwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>


CalWindow::CalWindow(Ui::MainWindow *ui)
{
    this->ui = ui;
    p = new Sequence();
    q = new Sequence();
    connect(ui->file1Btn, SIGNAL(clicked()), this, SLOT(openFile1()));
    connect(ui->file2Btn, SIGNAL(clicked()), this, SLOT(openFile2()));
    connect(ui->startBtn, SIGNAL(clicked()), this, SLOT(startSlot()));
    connect(ui->partDetailBtn, SIGNAL(clicked()), this, SLOT(showPartDetail()));
    connect(ui->pointDeatilBtn, SIGNAL(clicked()), this, SLOT(showPointDetail()));
    // ui->mapWidget->editJs();
    ui->mapWidget->initJS();
    ui->mapWidget->reload();
}

CalWindow::~CalWindow()
{

}

int CalWindow::calSeq()
{
    Core core;
    double res = core.computeDiscreteFrechet(p,q);
    if(res == 0)
    {
        QMessageBox::information(NULL, "提示", "输入轨迹完全重合", QMessageBox::Yes, QMessageBox::Yes);
        return -1;
    }
    ui->Result->setText(QString::number(res));
    return 1;
}

void CalWindow::calPoint()
{
    Core core;
    QVector<PointCompare> pc=core.getNearestPoint(p,q);
    pointDetail.setPointInfo(pc, p, q, namep, nameq);
    PointCompare pc1=pc[0];
    ui->bianhao1->setText(QString::number(1));
    ui->bianhao2->setText(QString::number(2));
    ui->xuliehao1->setText(QString::number(pc1.index1));
    ui->xuliehao2->setText(QString::number(pc1.index2));
    double lati1,lati2,longiti1,longiti2;
    lati1=p->pts[pc1.index1].latitude;
    lati2=q->pts[pc1.index2].latitude;
    longiti1=p->pts[pc1.index1].longitude;
    longiti2=q->pts[pc1.index2].longitude;

    ui->latitude1->setText(QString::number(lati1));
    ui->latitude2->setText(QString::number(lati2));
    ui->longitude1->setText(QString::number(longiti1));
    ui->longitude2->setText(QString::number(longiti2));
    ui->color1->setText("绿色");
    ui->color2->setText("绿色");
    if(!p->pts[pc1.index1].time.isEmpty()) {
        ui->time1->setText(p->pts[pc1.index1].time);
        ui->time2->setText(q->pts[pc1.index2].time);
    } else {
        ui->time1->setText("不含时间");
        ui->time2->setText("不含时间");
    }


    //DrawPoints()
    for (int i = 0; i < pc.length(); i++)
    {
        Point m =  p->pts[pc[i].index1];
        ui->mapWidget->drawPoint(&m,"p"+QString::number(i),6,true);
        Point n =  q->pts[pc[i].index2];
        ui->mapWidget->drawPoint(&n,"q"+QString::number(i),3,true);
    }
}

void CalWindow::openFile1()
{
    if (p->getNum() != 0)
    {
        delete p;
        p = new Sequence();
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
    else {
        return;
    }

    QFileInfo fi =   QFileInfo(file_name);
    namep = fi.fileName();


    ui->file1Path->setText(file_name);
    string fileName = file_name.toLocal8Bit().data();
    ifstream fin(fileName.c_str());
    Csv csv(fin);
    try
    {
        getSquFromFile(&csv,p);
    }
    catch(int i)
    {
        QMessageBox::information(NULL, "Error 错误代码" + QString::number(i), "时间格式错误,格式类似20160601 13:00:11", QMessageBox::Yes, QMessageBox::Yes);
        delete p;
        p = new Sequence();
        ui->file1Path->clear();
        fin.close();
        return;
    }
    p->setID("p");
    fin.close();
}

void CalWindow::openFile2()
{
    if (q->getNum() != 0)
    {
        delete q;
        q = new Sequence();
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
    else {
        return;
    }

    QFileInfo fi =   QFileInfo(file_name);
    nameq = fi.fileName();

    ui->file2Path->setText(file_name);
    string fileName = file_name.toLocal8Bit().data();
    ifstream fin(fileName.c_str());
    Csv csv(fin);
    try
    {
        getSquFromFile(&csv,q);
    }
    catch(int i)
    {
        QMessageBox::information(NULL, "Error 错误代码" + QString::number(i), "时间格式错误,格式类似20160601 13:00:11", QMessageBox::Yes, QMessageBox::Yes);
        delete q;
        q = new Sequence();
        ui->file2Path->clear();
        fin.close();
        return;
    }
    q->setID("q");
    fin.close();
}



void CalWindow::startSlot()
{
    // QTextStream cout(stdout,  QIODevice::WriteOnly);

    if (p->getNum()*q->getNum() == 0)
    {
        ui->mapWidget->initJS();
        ui->mapWidget->setDefaultCentralPt();
        ui->mapWidget->showPoints(true);
        ui->mapWidget->showTimes(true);
        ui->mapWidget->showEndPoints(true);
        return;
    }
    else
    {
        ui->mapWidget->initJS();
        ui->mapWidget->showPoints(true);
        ui->mapWidget->showTimes(true);
        ui->mapWidget->showEndPoints(true);
        //轨迹段计算部分
        QVector<Sequence> t;
        t.append(*p);
        t.append(*q);
        //设定地图中心点
        double x = (p->getCentralPoint().longitude + q->getCentralPoint().longitude)/2;
        double y = (p->getCentralPoint().latitude + q->getCentralPoint().latitude)/2;
        ui->mapWidget->setCentralPoint(x, y, getZoom(t));
        ui->mapWidget->drawSequences(t);
        //开始计算

        //整条轨迹计算部分
        int m=calSeq();
       if(m==-1){
         return;
       }
        //轨迹点计算部分
        calPoint();

        // int beginMin1,beginMin2;
        Core core;
        QVector<QVector<int> >qc=core.getSimplify(p,q);
        QVector<int>pv=qc[0];
        QVector<int>qv=qc[1];

        ui->start1->setText(QString::number(pv[0]));
        ui->end1->setText(QString::number(pv[1]));
        ui->start2->setText(QString::number(qv[0]));
        ui->end2->setText(QString::number(qv[1]));
        ui->g_bianhao1->setText("1");
        ui->g_bianhao2->setText("2");
        for(int i=0; i+1<pv.size(); i=i+2) {
            int begin1=pv[i];
            int end1=pv[i+1];
            int begin2=qv[i];
            int end2=qv[i+1];
            ui->mapWidget->highLightPart(p, begin1, end1, 3, 10);
            ui->mapWidget->highLightPart(q, begin2, end2, 3, 10);
        }
        qc.clear();
        ui->mapWidget->reload();
    }
}

void CalWindow::showPartDetail()
{
    if(p->pointsNum == 0)
    {
        return;
    }
    if(q->pointsNum == 0)
    {
        return;
    }
    pd.setWindowModality(Qt::ApplicationModal);
    pd.setPartInfo(p,q,namep,nameq);
    pd.show();
}

void CalWindow::showPointDetail()
{
    if(p->pointsNum == 0)
    {
        return;
    }
    if(q->pointsNum == 0)
    {
        return;
    }
    pointDetail.setWindowModality(Qt::ApplicationModal);
    pointDetail.show();
}
