#ifndef SEQUENCE_H
#define SEQUENCE_H


#include "header.h"
#include <cassert>
#include <iostream>

struct Time{
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
    void buildPoint(QString pt, QString te, QString ID);
    double longitude;
    double latitude;
    QString time;
    QString id;
    Time t;
};



class Sequence
{
private:
    QString id;

    int initSize;
    int seek;
    int getNumByID();
public:
    int pointsNum;
    Point *pts;
    Sequence(QString ID);
    Sequence();
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
};

#endif // SEQUENCE_H
