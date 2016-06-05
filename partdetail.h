#ifndef PARTDETAIL_H
#define PARTDETAIL_H

#include <QMainWindow>
#include "header.h"

namespace Ui {
class partDetail;
}

class partDetail : public QMainWindow
{
    Q_OBJECT

public:
    explicit partDetail(QWidget *parent = 0);
    ~partDetail();
    void setPartInfo(Sequence *p, Sequence *q, QString namep, QString nameq);


private:
    Ui::partDetail *ui;

    void initTable();

};

#endif // PARTDETAIL_H
