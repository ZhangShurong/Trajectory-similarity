/**
  *   赛题：第五届“中国软件杯”大学生软件设计大赛——针对以经纬度或经纬度带时间定义的不同轨迹
  *   软件名称：基于“弗雷歇距离”的轨迹相似度分析软件
  *   开发团队：Kryptonite
  *   开发时间：2016年4至6月
  *   文件功能概述：百度地图调用
  */

#ifndef MAPWINDOW_H
#define MAPWINDOW_H
#include "mainwindow.h"
#include <QWebView>
#include "header.h"
#include "sequence.h"


class MapWindow : public QWidget
{
private:
    bool showPoint;
    bool showTime;
    bool showEndpoints;
    int filterStatus;//0 only common;1 only time; others all
    int numOfSeq;//在draw sequence 中使用， 限制vector 中轨迹的条数


    QString test;
    QString jsFilePath;
    QString htmlFilePath;
    QVBoxLayout *mainLayout;
    QWebView *view;
    QFile *jsFile;

    Sequence *se_draw;//轨迹数组
    int se_num;//轨迹数量

    QStringList color;//轨迹颜色
    QStringList pointColor;//轨迹点颜色

    //储存变量名
    QStringList markList;
    QStringList labelList;

    //画点
    void drawPoints(Point *ps_a, int num, QString sqeID);
    void initColor();
    void initWidget();
    void initMap();
    void deleteJS();
    void draw();//Load html file
    void drawSqu(Sequence *se_a, int c = 3, int lWeight = 3);//One sequence
public:
    explicit MapWindow(QWidget *parent = 0);
    MapWindow(QString js, QString html, QWidget *parent = 0);
    ~MapWindow();

    void setJs(QString js);
    void setHtml(QString html);

    void clearVarList();
    void editJs();
    void initJS();
    void closeJS();

    void drawSequences(Sequence *se_a,int num);
    void drawSequences(QVector<Sequence> se_draw_a);
    void drawSequences(QVector<Sequence> se_draw_a, QStringList coin_list);
    void drawSequence(Sequence se_a, int c = 3, int lWeight = 3);
    void drawSequencePair(Sequence *se_a, Sequence *se_b, int c);

    void createFunc();

    void setCentralPoint(double x, double y,int zoom);
    void setCentralPoint(Point pt,int zoom);
    void setDefaultCentralPt();
    void setNumOfSeq(int n);

    void reload();

    void drawPoint(Point *p_a, QString pid, int color, bool big = false);
    void drawPoint(Point p_a, QString pid, int color, bool big = false);
    void drawPoint(Point *p_a, QString pid, int color, int num, bool big = false);
    void drawPoints(QVector<Point> points_a, int color, bool big = false);
    void showPoints(bool sp_a);
    void showTimes(bool st_a);
    void showEndPoints(bool se_a);
    void setFilter(int mod);


    void highLightPart(Sequence *se_a, int start, int end, int c, int lWeight = 5);
};
#endif // MAPWINDOW_H
