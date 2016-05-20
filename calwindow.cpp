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
   // ui->mapWidget->editJs();
    ui->mapWidget->initJS();
    ui->mapWidget->reload();

}

CalWindow::~CalWindow()
{

}

void CalWindow::openFile1()
{
    if (p->getNum() != 0)
    {
        Sequence *t = new Sequence();
        t = p;
        delete t;
        p = NULL;
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
        else{
            return;
    }
    ui->file1Path->setText(file_name);
    string fileName = file_name.toLocal8Bit().data();
    ifstream fin(fileName.c_str());
    Csv csv(fin);
    getSquFromFile(&csv,p);
    p->setID("p");
    fin.close();
}

void CalWindow::openFile2()
{
    if (q->getNum() != 0)
    {
        Sequence *t = new Sequence();
        t = q;
        delete t;
        q = NULL;
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
        else{
            return;
    }
    ui->file2Path->setText(file_name);
    string fileName = file_name.toLocal8Bit().data();
    ifstream fin(fileName.c_str());
    Csv csv(fin);
    getSquFromFile(&csv,q);
    q->setID("q");
    fin.close();
}



void CalWindow::startSlot()
{
   // QTextStream cout(stdout,  QIODevice::WriteOnly);
    ui->mapWidget->initJS();
    ui->mapWin->setDefaultCentralPt();
    ui->mapWidget->showPoints(true);
        if (p->getNum()*q->getNum() == 0)
        {
            return;
        }
        else
        {
            //设定地图中心点
            double x = (p->getCentralPoint().longitude + q->getCentralPoint().longitude)/2;
            double y = (p->getCentralPoint().latitude + q->getCentralPoint().latitude)/2;
            ui->mapWidget->setCentralPoint(x, y, 10);

            //开始计算


           QVector<QString>qst;
           QVector<Sequence> t;
           int beginMin1,beginMin2;

           double res = computeDiscreteFrechet(p,q);
           ui->Result->setText(QString::number(res));
           QVector<SecCompare> qs1=findBest(p,q,beginMin1,beginMin2);

           t.append(*p);
           t.append(*q);
           ui->mapWidget->drawSequences(t);

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

            for(int i=0;i<qst.length();i++){
                QStringList ql=qst[i].split(" ");
                int begin1=ql[0].toInt();
                int end1=ql[1].toInt();
                int begin2=ql[2].toInt();
                int end2=ql[3].toInt();
                ui->mapWidget->highLightPart(p, begin1+beginMin1, end1+beginMin1, 7, 10);
                ui->mapWidget->highLightPart(q, begin2+beginMin2, end2+beginMin2, 7, 10);
                //cout<<"qst"<<begin1<<" "<<end1<<" "<<begin2<<" "<<end2<<endl;
            }


            ui->mapWidget->reload();
        }
}
