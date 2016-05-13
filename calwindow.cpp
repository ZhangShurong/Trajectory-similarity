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
//        Sequence *t = new Sequence();
//        t = p;
//        delete t;
//        p = NULL;
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
//        Sequence *t = new Sequence();
//        t = q;
//        delete t;
//        q = NULL;
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
    QTextStream cout(stdout,  QIODevice::WriteOnly);        
        ui->mapWidget->initJS();
        ui->mapWin->setDefaultCentralPt();
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

            QVector<SecCompare> temp = getBestSce(findSimilarSection(p, q,1),1);
            int beginMin1=temp[0].beginIndex1;
            int endMax1=temp[0].endIndex1;
            int beginMin2=temp[0].beginIndex2;
            int endMax2=temp[0].endIndex2;

            for (int i = 0; i < temp.length(); i++)
            {
               if(temp[i].beginIndex1<=beginMin1)
                   beginMin1=temp[i].beginIndex1;
               if(temp[i].beginIndex2<=beginMin2)
                   beginMin2=temp[i].beginIndex2;
               if(temp[i].endIndex1>endMax1)
                   endMax1=temp[i].endIndex1;
               if(temp[i].endIndex2>endMax2)
                   endMax2=temp[i].endIndex2;
            }

            Sequence m1;
            Sequence m2;

            m1.pointsNum=endMax1-beginMin1+1;
            m2.pointsNum=endMax2-beginMin2+1;

            cout<<m1.pointsNum<<" "<<m2.pointsNum<<endl;

             m1.pts = new Point[m1.pointsNum];
             m2.pts = new Point[m2.pointsNum];

            for(int i=beginMin1;i<=endMax1;i++){
                m1.pts[i-beginMin1]=p->pts[i];
            }
            for(int i=beginMin2;i<=endMax2;i++){
                m2.pts[i-beginMin2]=q->pts[i];
            }

           QVector<SecCompare>qs= getBestSce(findSimilarSection(&m1,&m2,2),2);
           QVector<QString>qst;
           QVector<Sequence> t;
           t.append(*p);
           t.append(*q);
           ui->mapWidget->drawSequences(t);
            if (qs.length() != 0)
            {
                for (int i = 0; i < qs.length(); i++)
                {
                    QString s = QString::number(qs[i].beginIndex1, 10)+" "+ QString::number(qs[i].endIndex1, 10)+" "+QString::number(qs[i].beginIndex2, 10)+" "+QString::number(qs[i].endIndex2, 10);
                   if(!qst.contains(s))
                     qst.append(s);
                }
            }

            for(int i=0;i<qst.length();i++){
               // int begin1=qst[i].spl
                QStringList ql=qst[i].split(" ");
                int begin1=ql[0].toInt();
                int end1=ql[1].toInt();
                int begin2=ql[2].toInt();
                int end2=ql[3].toInt();
                ui->mapWidget->highLightPart(p, begin1+beginMin1, end1+beginMin1, 7, 10);
                ui->mapWidget->highLightPart(q, begin2+beginMin2, end2+beginMin2, 7, 10);
                cout<<"qst"<<begin1<<" "<<end1<<" "<<begin2<<" "<<end2<<endl;
            }
            ui->mapWidget->reload();
        }
}

