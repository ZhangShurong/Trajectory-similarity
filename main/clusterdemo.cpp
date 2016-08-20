#include "clusterdemo.h"
#include <QPoint>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>


ClusterDemo::ClusterDemo(QWidget *parent) : QWidget(parent)
{
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::StrongFocus);
    this->isPressed = false;
    this->pointsNum = 0;
    this->index = 0;
    this->clustered = false;
}

void ClusterDemo::paintEvent(QPaintEvent */*p*/)
{
    QPainter painter(this);
    painter.drawPixmap(0,0,this->width(),this->height(),QPixmap(":/images/roundabout.png"));
    QPen pen;                                 //创建一个画笔
    pen.setColor(Qt::black);
    pen.setWidth(5);
    pen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    for(uint i = 0;i<lines.size();i++){
        Line pLine = lines[i];
        painter.drawLine(pLine.startPnt,pLine.endPnt);
    }
    for(uint i = 0; i < points.size(); i++)
    {
        pen.setWidth(10);
        painter.setPen(pen);
        painter.drawPoint(points[i]);
    }
    if(clustered)
    {
        for(uint i = 0; i < sequences.size(); i++)
        {
            pen.setWidth(5);
            if(res.at(i) == 0)
            {
                pen.setColor(Qt::red);
            }
            else if(res.at(i) == 2)
            {
                pen.setColor(Qt::blue);
            }
            else
            {
                pen.setColor(Qt::yellow);
            }
            painter.setPen(pen);
            for(int j = 0; j < sequences.at(i).pointsNum; j++)
            {
                painter.drawPoint(sequences.at(i).pts[j].longitude,sequences.at(i).pts[j].latitude);
            }
        }
    }
}

void ClusterDemo::mousePressEvent(QMouseEvent *e)
{
    clustered = false;
    setCursor(Qt::PointingHandCursor);
    startPnt = e->pos();
    endPnt = e->pos();
    this->isPressed = true;
    this->pointsNum ++;
    points.push_back(startPnt);
}

void ClusterDemo::mouseMoveEvent(QMouseEvent *e)
{
    if(this->isPressed){
        endPnt = e->pos();
        Line line;  //put the new line into vector
        line.startPnt = startPnt;
        line.endPnt = endPnt;
        this->lines.push_back(line);
        update();                                    //repainter，call paintEvent
        startPnt = endPnt;
        pointsNum ++;
        if(pointsNum %5 == 0)
        {
            points.push_back(startPnt);
        }
    }
}

void ClusterDemo::mouseReleaseEvent(QMouseEvent */*e*/)
{
    points.push_back(startPnt);
    setCursor(Qt::ArrowCursor);
    this->isPressed = false;
    update();

    Sequence t;
    for(size_t i = index; i < points.size(); i++)
    {
        t.appendPt(new Point(points[i].x(), points[i].y()));
    }
    index = points.size();
    sequences.push_back(t);
}

void ClusterDemo::keyPressEvent(QKeyEvent *e)
{
    if(e->key()==Qt::Key_R)
    {
        reset();
    }
    if(e->key()==Qt::Key_A)
    {
        cluster();
    }
}

ClusterDemo::~ClusterDemo()
{

}

void ClusterDemo::reset()
{
    temp.clear();
    index = 0;
    pointsNum = 0;
    res.clear();
    points.clear();
    sequences.clear();
    lines.clear();
    update();
}

void ClusterDemo::cluster()
{
    if(sequences.size() != 0)
    {
        Core core;
        res = core.clusterAgglomerartive(sequences);
        qDebug() << res.size();
        clustered = true;
        update();
    }
}

