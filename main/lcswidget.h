/**
  *   赛题：第五届“中国软件杯”大学生软件设计大赛——针对以经纬度或经纬度带时间定义的不同轨迹
  *   软件名称：基于“弗雷歇距离”的轨迹相似度分析软件
  *   开发团队：Kryptonite
  *   开发时间：2016年4至6月
  *   文件功能概述：简易轨迹段匹配
  */

#ifndef LCSWIDGET_H
#define LCSWIDGET_H

#include <QWidget>
#include <QSpinBox>
#include <QLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QAction>
#include <QToolBar>
#include <QFile>
#include <QTextStream>
#include "core.h"
#include "mapwindow.h"

class AttrEdit : public QWidget
{
    Q_OBJECT
public:
    QLabel 		*attrLabel;
    QLineEdit 	*valLineEdit;

    explicit AttrEdit(QWidget *parent, const QString &attrName = "",
                      bool ro = true) : QWidget(parent) {
        QHBoxLayout *mainLayout = new QHBoxLayout;
        attrLabel = new QLabel(attrName);
        valLineEdit = new QLineEdit;
        valLineEdit->setReadOnly(ro);
        mainLayout->addWidget(attrLabel);
        mainLayout->addSpacing(5);
        mainLayout->addWidget(valLineEdit);
        this->setLayout(mainLayout);
    }

    void setLabelText(const QString &str) {
        attrLabel->setText(str);
    }
};

class LcsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LcsWidget(QWidget *parent = 0);

private:
    void setupUi();
    void setupActions();

    MapWindow   *map;

    QSlider     *thresholdSlider;
    QPushButton *refreshButton;

    QGroupBox   *groupBox;
    AttrEdit    *attrThreshold;
    AttrEdit	*attrOriginSimilarity;
    AttrEdit	*attrCommonSimilarity;
    AttrEdit	*attrOriginPointCount[2];
    AttrEdit 	*attrCommonPointCount;

    QSpinBox    *upperLimBox;
    QSpinBox    *lowerLimBox;

    QPushButton *fileOpenButtion[2];
    QLabel      *fileNameLabel[2];

    QAction		*exportAction[2];
    QToolBar    *toolBar[2];

    /****** Non-UI part ********/
    Sequence *raw_seq[2], *common_seq[2];
    double threshold;

signals:

public slots:
    void openFile(int i);
    void exportFile(int i);
    void calcLcmSequence();
    void drawSequences();
    void onLowerLimChanged(int value);
    void onUpperLimChanged(int value);
    void onRefreshButtonClicked();
    void updateThreshold();
};

#endif // LCSWIDGET_H
