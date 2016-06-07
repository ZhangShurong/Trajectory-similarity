#include "detailwin.h"
#include "ui_detailwin.h"

detailWin::detailWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::detailWin)
{
    ui->setupUi(this);
    initTable();
}

void detailWin::showSeq(Sequence se)
{
    ui->mainTable->clearContents();
    if (se.getNum() == 0)
        return;
    else {
        for(int i = 0; i < se.getNum(); i++)
        {
            ui->mainTable->setItem(i,0, new QTableWidgetItem(se[i].id));
            ui->mainTable->setItem(i,1,
                                   new QTableWidgetItem(QString::number(se[i].longitude)));
            ui->mainTable->setItem(i,2,
                                   new QTableWidgetItem(QString::number(se[i].latitude)));
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
