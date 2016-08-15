#include "partdetail.h"
#include "ui_partdetail.h"
#include "core.h"
#define ROWCOUNT 100

partDetail::partDetail(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::partDetail)
{
    ui->setupUi(this);
    initTable();
}

partDetail::~partDetail()
{
    delete ui;
}

void partDetail::setPartInfo(Sequence *p, Sequence *q, QString namep, QString nameq)
{
    Core core;
    QVector<QVector<int> >qc=core.getSimplify(p,q);
    QVector<int>pv=qc[0];
    QVector<int>qv=qc[1];

    core.initP_Q(p, q);
    core.initMemSpace(p, q);
   // std::cout<<"time: "<<pv.size()<<std::endl;
    for(int i=0; i+1<pv.size(); i=i+2) {
        int begin1=pv[i];
        int end1=pv[i+1];
        int begin2=qv[i];
        int end2=qv[i+1];
        ui->mainTable->setItem(i,0, new QTableWidgetItem(namep));
        ui->mainTable->setItem(i,3, new QTableWidgetItem(nameq));
        ui->mainTable->setItem(i,1, new QTableWidgetItem(QString::number(begin1)));
        \
        ui->mainTable->setItem(i,2, new QTableWidgetItem(QString::number(end1)));
        ui->mainTable->setItem(i,4, new QTableWidgetItem(QString::number(begin2)));
        ui->mainTable->setItem(i,5, new QTableWidgetItem(QString::number(end2)));

        double res = core.computeDFD_new(begin1, end1, begin2, end2);
        ui->mainTable->setItem(i,6, new QTableWidgetItem(QString::number(res)));

    }
    ui->mainTable->sortItems(6);

}

void partDetail::initTable()
{
    ui->mainTable->horizontalHeader()->setStretchLastSection(true);
    ui->mainTable->verticalHeader()->hide();

    ui->mainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->mainTable->setColumnCount(7);
    ui->mainTable->setRowCount(ROWCOUNT);
    ui->mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList header;
    header << "所属轨迹1"
           << "起点"
           << "终点"
           << "所属轨迹2"
           << "起点"
           << "终点"
           << "相似系数（弗雷歇）";
    ui->mainTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->mainTable->setHorizontalHeaderLabels(header);
}
