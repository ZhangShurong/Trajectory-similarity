/**
  *   赛题：第五届“中国软件杯”大学生软件设计大赛——针对以经纬度或经纬度带时间定义的不同轨迹
  *   软件名称：基于“弗雷歇距离”的轨迹相似度分析软件
  *   开发团队：Kryptonite
  *   开发时间：2016年4至6月
  *   文件功能概述：核心算法实现文件，包含轨迹(带及不带时间)相似度计算、轨迹(带及不带时间)相似轨迹段查询、
  *             轨迹(带及不带时间)相似轨迹点查询的实现函数
  */

#ifndef CORE_H
#define CORE_H

#include <QString>
#include <QFileDialog>
#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include "sequence.h"
#include "csv.h"

#define EARTH_RADIUS 6378.137
#define PI 3.14
#define MINLONG 73
#define MAXLONG 136
#define MINLA 3
#define MAXLA 54



class Sequence;
class Point;
class Time;

struct PointCompare{
    int index1;
    int index2;
    double distance;
};

struct SecCompare{
    int beginIndex1;//起点1
    int endIndex1;//终点1
    int beginIndex2;
    int endIndex2;
    double simliarity;//轨迹段相似度
};

class Core{
    double **mem = NULL;
    Sequence *p;
    Sequence *q;
    double coef;
    int mX;
    int mY;
public:
    Core();
    ~Core();

    int tracsLen;

    void initP_Q(Sequence *m, Sequence *n);
    void initMemSpace(Sequence *p, Sequence *q);

    double computeDiscreteFrechet(Sequence *sa,Sequence *sb);//计算Frechet Distance
    double computeDFD(int i,int j, Sequence *p_a, Sequence * q_a);

    double  getSecSim(int i1,int j1,int i2,int j2);//计算轨迹段相似度

    void mergeChange(int gap,QVector<SecCompare> &q1,QVector<SecCompare> &q2,double limit);
    QVector<SecCompare> findSimilarSection(Sequence *se_a, Sequence *se_b);//调用以计算轨迹段
    void calculateSec(int gap, int h,QVector<SecCompare> &q1);
    Sequence* longestCommonSeq(Sequence &p, Sequence &q, double thres);
    double calCoef();
    QVector<SecCompare> findBest(Sequence* p,Sequence* q);
    QVector<QVector<int> > getSimplify(Sequence* p,Sequence* q);

    double computeDFD_new(int startx, int endx, int starty, int endy);
    double euclideanDistance(Point* a, Point* b);
    QVector<SecCompare> getBestSce(QVector<SecCompare> secCompareV_a);
    QVector<PointCompare> getNearestPoint(Sequence *se_a, Sequence *se_b);

    double modHausDist(Sequence*, Sequence*);
    void clusterAgglomerartive(Sequence *seqs, int len);
    bool compareType(QString input_type,QString type);
    void normalize(Sequence &se);
    void clusterSpectral();
    vector<int> clusterAgglomerartive(vector<Sequence> seqs);

    double calTimeDistance(Point *a,Point *b);
};
double rad(double d);
int calZoomCoef(double res);
int getZoom(Sequence seq_a);
Time loadToStruct(QString time);
int getZoom(QVector<Sequence> seqV);
bool timeCompare(Point*p1,Point*p2);
double euDistance(Point a, Point b);
bool compare(SecCompare s1,SecCompare s2);
void getSquFromFile(Csv *csv, Sequence *se);
Point getCenterPoint(QVector<Sequence> seqV);
Point getCenterPoint(Sequence *se_a, int num);
bool compareDis(PointCompare p1,PointCompare p2);
double getDistance(double lng1, double lat1, double lng2, double lat2);
int hardCluster(Sequence * q,double minLongtitude,
                double maxLongtitude,
                double minLatitude,
                double maxLatitude,
                int depth);
#endif // CORE_H
