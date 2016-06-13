/**
  *   赛题：第五届“中国软件杯”大学生软件设计大赛——针对以经纬度或经纬度带时间定义的不同轨迹
  *   软件名称：基于“弗雷歇距离”的轨迹相似度分析软件
  *   开发团队：Kryptonite
  *   开发时间：2016年4至6月
  *   文件功能概述：轨迹点详细信息
  */

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
