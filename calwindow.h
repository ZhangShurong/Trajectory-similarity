/**
  *   赛题：第五届“中国软件杯”大学生软件设计大赛——针对以经纬度或经纬度带时间定义的不同轨迹
  *   软件名称：基于“弗雷歇距离”的轨迹相似度分析软件
  *   开发团队：Kryptonite
  *   开发时间：2016年4至6月
  *   文件功能概述：两条轨迹的相似度计算界面，用于处理计算模块，ui作为参数从mainwindow传入
  */

#ifndef CALWINDOW_H
#define CALWINDOW_H
#include <QMainWindow>
#include "header.h"
#include "sequence.h"
#include <QEvent>
#include <QCloseEvent>
#include <string>
#include "core.h"
#include "partdetail.h"
#include "pointdetail.h"

namespace Ui {
class MainWindow;
}
class MapWindow;

class CalWindow : public QObject
{
    Q_OBJECT
public:
    CalWindow(Ui::MainWindow *ui);
    ~CalWindow();
private:
    Ui::MainWindow *ui;
    Sequence *p;
    Sequence *q;
    QString namep;
    QString nameq;
    partDetail pd;
    PointDetail pointDetail;
    int calSeq();
    void calPoint();


protected:

public slots:
    void openFile1();
    void openFile2();
    void startSlot();
    void showPartDetail();
    void showPointDetail();
};

#endif // CALWINDOW_H
