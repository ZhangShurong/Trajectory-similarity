/*
 @header     detailwin.h

 @author     张树荣

 @abstract   显示轨迹详细信息

 @discussion 传入轨迹数据，其详细信息会以表格形式显示

*/

#ifndef DETAILWIN_H
#define DETAILWIN_H

#include <QMainWindow>
#include "header.h"
#include "sequence.h"
namespace Ui {
class detailWin;
}

class detailWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit detailWin(QWidget *parent = 0);
    void showSeq(Sequence se);
    ~detailWin();

private:
    Ui::detailWin *ui;
    void initTable();
};

#endif // DETAILWIN_H
