/**
  *   赛题：第五届“中国软件杯”大学生软件设计大赛——针对以经纬度或经纬度带时间定义的不同轨迹
  *   软件名称：基于“弗雷歇距离”的轨迹相似度分析软件
  *   开发团队：Kryptonite
  *   开发时间：2016年4至6月
  *   文件功能概述：网络状态监测
  */


#ifndef NETSTATINDICATOR_H
#define NETSTATINDICATOR_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QUrl>
#include <QTcpSocket>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QTimer>

class NetStatIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit NetStatIndicator(QWidget *parent = 0);
private:
    static const int timeout = 2 * 1000; //ms
    static const int interval = 6 * 1000; //ms
    bool   alerted;
    double netspeed;
    QTimer timer;
    QLabel *netStatLabel;
    QLabel *netspeedLabel;

    static QString toHumanSpeed(double speed);
    static QString toDescription(double speed);

    static const QString okStyleStr;
    static const QString badStyleStr;
signals:

public slots:
    void updateNetStat();
    void updateUi();
    void alertProperly();
};


#endif // NETSTATINDICATOR_H
