/**
  *   赛题：第五届“中国软件杯”大学生软件设计大赛——针对以经纬度或经纬度带时间定义的不同轨迹
  *   软件名称：基于“弗雷歇距离”的轨迹相似度分析软件
  *   开发团队：Kryptonite
  *   开发时间：2016年4至6月
  *   文件功能概述：简易轨迹段匹配
  */

#ifndef LCMWIDGET_H
#define LCMWIDGET_H

#include <QWidget>
#include <QSpinBox>
#include <QLayout>
#include "mapwindow.h"

class lcmWidget : public QWidget
{
    Q_OBJECT
public:
    explicit lcmWidget(QWidget *parent = 0);

private:
    void setupUi();
    void setupActions();

    MapWindow   *map;

    QSlider     *thresholdSlider;
    QLabel      *thresholdLabel;
    QPushButton *refreshButton;

    QSpinBox    *upperLimBox;
    QSpinBox    *lowerLimBox;

    QPushButton *fileOpenButtion[2];
    QLabel      *fileNameLabel[2];

    /****** Non-UI part ********/
    Sequence *raw_seq[2], *common_seq[2];
    double threshold;

signals:

public slots:
    void openFile(int i);
    void calcLcmSequence();
    void drawSequences();
    void onLowerLimChanged(int value);
    void onUpperLimChanged(int value);
    void onRefreshButtonClicked();
    void updateThreshold();
};


#endif // LCMWIDGET_H
