#ifndef POINTDETAIL_H
#define POINTDETAIL_H

#include <QMainWindow>
#include "core.h"
namespace Ui {
class PointDetail;
}

class PointDetail : public QMainWindow
{
    Q_OBJECT

public:
    explicit PointDetail(QWidget *parent = 0);
    ~PointDetail();
    void setPointInfo(QVector<PointCompare> compareV,Sequence *p, Sequence *q ,QString namep, QString nameq);

private:
    Ui::PointDetail *ui;
    void initTable();
};

#endif // POINTDETAIL_H
