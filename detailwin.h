/**
  *   赛题：第五届“中国软件杯”大学生软件设计大赛——针对以经纬度或经纬度带时间定义的不同轨迹
  *   软件名称：基于“弗雷歇距离”的轨迹相似度分析软件
  *   开发团队：Kryptonite
  *   开发时间：2016年4至6月
  *   文件功能概述：轨迹详细信息查看,传入轨迹数据，其详细信息会以表格形式显示
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
