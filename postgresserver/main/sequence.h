/**
  *   赛题：第五届“中国软件杯”大学生软件设计大赛——针对以经纬度或经纬度带时间定义的不同轨迹
  *   软件名称：基于“弗雷歇距离”的轨迹相似度分析软件
  *   开发团队：Kryptonite
  *   开发时间：2016年4至6月
  *   文件功能概述：轨迹及轨迹点类结构定义
  */

#ifndef SEQUENCE_H
#define SEQUENCE_H


#include "header.h"
#include <cassert>
#include <iostream>
#include <string.h>
#include <QDataStream>
#include <QtNetwork>

struct Time
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
};

class Point
{
public:
    Point();
    Point(double x, double y, QString time_a = "", QString id_a = "");
   // Point(Point &p);
    void buildPoint(QString pt, QString te, QString ID);
    double longitude;
    double latitude;
    QString time;
    QString id;
    Time t;
    bool painted;
    friend QDataStream &operator >>(QDataStream &in, Point &p)
    {
        in >> p.longitude >> p.latitude >> p.time >> p.id;
        return in;
    }

    friend QDataStream &operator <<(QDataStream &out, const Point &p)
    {
        out << p.longitude << p.latitude << p.time << p.id;
        return out;
    }
};

class Sequence
{
private:
    QString id;
    int initSize;
    int seek;
    int getNumByID();
    void buildPrefixSum();
    std::vector<double> prefixSum;
    QString type;
public:
    int pointsNum;
    Point *pts;
    Sequence(QString ID);
   // Sequence(Sequence &se);
    Sequence();
    void initPainted();
    void clear();//Some problem
    void appendPt(Point *pt);
    void setPtNum(int num);
    void setPoints(Point *points);
    void outputTrac();
    int getNum();
    void setID(QString ID);
    QString getID();
    Point getCentralPoint();
    double getMaxX();
    double getMaxY();
    double getMinX();
    double getMinY();
    Point &operator[](size_t index) {
        assert(index < (size_t) pointsNum);
        return pts[index];
    }
    int getIndex(Point pt);
    bool hasTime();
    double getPrefixSum(int i);
    void setType(QString type)
    {
        this->type = type;
    }
    QString getType()
    {
        return type;
    }
    friend QDataStream &operator >>(QDataStream &in, Sequence &s)
    {
        in >> s.id >> s.initSize >> s.seek >> s.type >> s.pointsNum;
        s.pts = new Point[s.pointsNum];
        for(int i = 0; i < s.pointsNum ;i++)
        {
            in >> s.pts[i];
        }
        return in;
    }

    friend QDataStream &operator <<(QDataStream &out, const Sequence &s)
    {
        out << s.id << s.initSize << s.seek << s.type << s.pointsNum;
        for(int i = 0; i < s.pointsNum ;i++)
        {
            out << s.pts[i];
        }
        return out;
    }
};
#endif // SEQUENCE_H
