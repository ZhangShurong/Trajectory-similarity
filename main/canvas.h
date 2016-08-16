/**
  *   赛题：第五届“中国软件杯”大学生软件设计大赛——针对以经纬度或经纬度带时间定义的不同轨迹
  *   软件名称：基于“弗雷歇距离”的轨迹相似度分析软件
  *   开发团队：Kryptonite
  *   开发时间：2016年4至6月
  *   文件功能概述：离线地图界面，继承于Qwidget,用于绘制轨迹， 传入轨迹数据，调用void setSequences(Sequence *se_a,int n);来绘制轨迹
  */

#ifndef CANVAS_H
#define CANVAS_H
#include "core.h"
#include <QWidget>
#include <vector>
#include <QEvent>
#include <QMouseEvent>


class Canvas : public QWidget
{
    Q_OBJECT
private:
    double minX;
    double minY;
    double maxX;
    double maxY;
    //many sequences
    Sequence *se_draw;
    int se_num;

    QPainter *painter;
    QPen *pen;
    int xresolution;
    int yresolution;
    void drawGrid();
    void changeRES(int width,int height);
    QPoint translateCo(double x, double y);
    void drawAxies();
    void getRange();
    void drawSeq(Sequence *se_a,int i);

public:
    explicit Canvas(QWidget *parent = 0);
    void setSequences(Sequence *se_a,int n);
    //void drawSeq(Sequence *se_a, QColor c, int weight);
protected:
    void paintEvent(QPaintEvent *event);
signals:

public slots:
};

#endif // CANVAS_H

