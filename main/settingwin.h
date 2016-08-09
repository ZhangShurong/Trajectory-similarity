/**
  *   赛题：第五届“中国软件杯”大学生软件设计大赛——针对以经纬度或经纬度带时间定义的不同轨迹
  *   软件名称：基于“弗雷歇距离”的轨迹相似度分析软件
  *   开发团队：Kryptonite
  *   开发时间：2016年4至6月
  *   文件功能概述：配置文件处理
  */

#ifndef SETTINGWIN_H
#define SETTINGWIN_H

#include <QObject>
#include <QSettings>


namespace Ui {
class MainWindow;
}

class SettingWin : public QObject
{
    Q_OBJECT
public:
    explicit SettingWin(Ui::MainWindow *ui, QObject *parent = 0);
    ~SettingWin();
private:
    Ui::MainWindow *ui;
    QSettings *setting;
    QString settingPath;
    QMap<QString, int> settingMap;
    //int seqLimit;
    void initComBox();
    bool readIni();
    void initSettingMap();
signals:

public slots:
};

#endif // SETTINGWIN_H
