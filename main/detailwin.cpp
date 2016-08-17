#include "detailwin.h"
#include "ui_detailwin.h"

detailWin::detailWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::detailWin)
{
    ui->setupUi(this);
    this->setWindowTitle("轨迹详情");
    initTable();
}

void detailWin::showSeq(Sequence se)
{
    ui->mainTable->clearContents();
    if (se.getNum() == 0)
        return;
    else {
        char  buffer1[200];
        char  buffer2[200];
        QString str1;
        QString str2;
        for(int i = 0; i < se.getNum(); i++)
        {

            sprintf(buffer1,"%.6lf",se[i].longitude);
            sprintf(buffer2,"%.6lf",se[i].latitude);
            str1 = QString(QLatin1String(buffer1));
            str2 = QString(QLatin1String(buffer2));
            ui->mainTable->setItem(i,0, new QTableWidgetItem(se[i].id));
            ui->mainTable->setItem(i,1,
                                   new QTableWidgetItem(str1));
            ui->mainTable->setItem(i,2,
                                   new QTableWidgetItem(str2));
            ui->mainTable->setItem(i,3,
                                   new QTableWidgetItem(se[i].time));
        }
    }
}

detailWin::~detailWin()
{
    delete ui;
}

void detailWin::initTable()
{
    ui->mainTable->horizontalHeader()->setStretchLastSection(true);
    ui->mainTable->verticalHeader()->hide();

    ui->mainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->mainTable->setColumnCount(4);
    ui->mainTable->setRowCount(100);
    ui->mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList header;
    header << tr("PID")
           << tr("Longtitude")
           << tr("Latitude")
           << tr("Time");
    ui->mainTable->setHorizontalHeaderLabels(header);
}
