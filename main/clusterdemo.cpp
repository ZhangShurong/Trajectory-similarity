#include "clusterdemo.h"
#include <QPoint>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>


ClusterDemo::ClusterDemo(QWidget *parent) : QWidget(parent)
{
    this->setMouseTracking(true);
    this->isPressed = false;
}

void ClusterDemo::paintEvent(QPaintEvent /**p*/)
{
    QPainter painter(this);
    QPen pen;                                 //创建一个画笔
    pen.setColor(Qt::black);
    pen.setWidth(5);
    pen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    for(uint i = 0;i<lines.size();i++){
        Line pLine = lines[i];
        painter.drawLine(pLine.startPnt,pLine.endPnt);
        if(i%10 == 0)
        {
            pen.setWidth(10);
            painter.setPen(pen);
            painter.drawPoint(pLine.startPnt);
            pen.setWidth(5);
            painter.setPen(pen);
        }
    }
}

void ClusterDemo::mousePressEvent(QMouseEvent *e)
{
    setCursor(Qt::PointingHandCursor);
    startPnt = e->pos();
    endPnt = e->pos();
    this->isPressed = true;
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
    }
}

void ClusterDemo::mouseReleaseEvent(QMouseEvent)
{
    setCursor(Qt::ArrowCursor);
    this->isPressed = false;
}

ClusterDemo::~ClusterDemo()
{

}

