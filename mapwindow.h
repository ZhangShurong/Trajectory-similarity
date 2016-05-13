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
    void clearVarList();

    void editJs();
    void initJS();
    void closeJS();

    void drawSequences(Sequence *se_a,int num);
    void drawSequences(QVector<Sequence> se_draw_a);
    void drawSequence(Sequence se_a, int c = 3, int lWeight = 3);

    void createFunc();

    void setCentralPoint(double x, double y,int zoom);
    void setCentralPoint(Point pt,int zoom);
    void setDefaultCentralPt();

    void reload();

    void drawPoint(Point *p_a, QString pid, int color);
    void showPoints(bool sp_a);
    void showTimes(bool st_a);

    void highLightPart(Sequence *se_a, int start, int end, int c, int lWeight = 5);
};
#endif // MAPWINDOW_H
