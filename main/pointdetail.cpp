#include "pointdetail.h"
#include "ui_pointdetail.h"
#define ROWCOUNT 100

PointDetail::PointDetail(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PointDetail)
{
    ui->setupUi(this);
    initTable();
}

PointDetail::~PointDetail()
{
    delete ui;
}

void PointDetail::setPointInfo(QVector<PointCompare> compareV,Sequence *p, Sequence *q ,QString namep, QString nameq)
{
    for(int i = 0; i < compareV.size(); i++)
    {
        int index1 = compareV[i].index1;
        int index2 = compareV[i].index2;
        QString str = QString::number(p->pts[index1].longitude) + "-" + QString::number(p->pts[index1].latitude);
        QString time = p->pts[i].time;
        ui->mainTable->setItem(i,0, new QTableWidgetItem(namep));
        ui->mainTable->setItem(i,1, new QTableWidgetItem(QString::number(index1)));
        ui->mainTable->setItem(i,2, new QTableWidgetItem(str));
        ui->mainTable->setItem(i,3, new QTableWidgetItem(time));

        str = QString::number(q->pts[index2].longitude) + "-" + QString::number(q->pts[index2].latitude);
        time = q->pts[i].time;
        ui->mainTable->setItem(i, 4, new QTableWidgetItem(nameq));
        ui->mainTable->setItem(i, 5, new QTableWidgetItem(QString::number(index2)));
        ui->mainTable->setItem(i, 6, new QTableWidgetItem(str));
        ui->mainTable->setItem(i, 7, new QTableWidgetItem(time));

        ui->mainTable->setItem(i, 8, new QTableWidgetItem(QString::number(compareV[i].distance)));

    }
}

void PointDetail::initTable()
{
    ui->mainTable->horizontalHeader()->setStretchLastSection(true);
    ui->mainTable->verticalHeader()->hide();

    ui->mainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->mainTable->setColumnCount(9);
    ui->mainTable->setRowCount(ROWCOUNT);
    ui->mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList header;
    header << "所属轨迹1"
           << "点序号"
           << "经纬度"
           << "时间"
           << "所属轨迹2"
           << "点序号"
           << "经纬度"
           << "时间"
           << "欧几里得距离";
    ui->mainTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->mainTable->setHorizontalHeaderLabels(header);
}
