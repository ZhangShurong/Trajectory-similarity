#include "canvas.h"
#include <QPainter>
#define PADDING 40
/*
 *
 * China:
 * longtitude: 75E to 135E
 * latitude: 5N to 54N
 * transform:
 * y = w/60(x - 75)
 * y = -h/50(x - 55)
 *
 */


void Canvas::drawGrid()
{
    pen->setColor(Qt::gray);
    pen->setStyle(Qt::DashLine);
    pen->setWidth(1);
    painter->setPen(*pen);
    QPoint start;
    QPoint end;

    int width = this->width() - PADDING;
    int height = this->height() - PADDING;
    changeRES(width - PADDING,height- PADDING);
}

void Canvas::changeRES(int width, int height)
{
    if (width >= 1000)
    {
        xresolution = 20;
    }
    else if (width >= 800)
    {
        xresolution = 15;
    }
    else
    {
        xresolution = 10;
    }
    if (height >=1000)
    {
        yresolution = 20;
    }
    else if (height >= 800)
    {
        yresolution = 15;
    }
    else
    {
        yresolution = 10;
    }

}

void Canvas::drawSeq(Sequence *se_a,int i)
{
    if(se_a->getNum() == 0)
    {
        return;
    }
    getRange();
    QVector<QPoint> points;
    if(i==0){
         pen->setColor(Qt::gray);
    }else{
         pen->setColor(Qt::blue);
    }
    pen->setWidth(3);
    pen->setJoinStyle(Qt::MiterJoin);
    pen->setStyle(Qt::SolidLine);
    painter->setPen(*pen);

    for (int i = 0; i<se_a->getNum(); i ++ )
    {
        QPoint temp =  translateCo(se_a->pts[i].longitude, se_a->pts[i].latitude);
        points.append(temp);
    }
    for (int i = 0; i<se_a->getNum() - 1; i++)
    {
        painter->drawLine(points[i], points[i+1]);
    }
    for (int i = 0; i<se_a->getNum(); i ++ )
    {
        pen->setColor(Qt::black);
        pen->setWidth(3);
        painter->setPen(*pen);
        QPoint temp =  translateCo(se_a->pts[i].longitude, se_a->pts[i].latitude);
        painter->drawPoint(temp);
        points.append(temp);
    }
}

//void Canvas::drawSeq(Sequence *se_a, QColor c, int weight)
//{
//    getRange();
//    QVector<QPoint> points;
//    for (int i = 0;i<se_a->getNum();i ++ )
//    {
//        pen->setColor(c);
//        pen->setWidth(weight + 2);
//        painter->setPen(*pen);
//        QPoint temp =  translateCo(se_a->pts[i].longitude, se_a->pts[i].latitude);
//        painter->drawPoint(temp);
//        points.append(temp);
//    }
//    pen->setColor(c);
//    pen->setWidth(weight);
//    pen->setJoinStyle(Qt::MiterJoin);
//    pen->setStyle(Qt::SolidLine);
//    painter->setPen(*pen);
//    for (int i = 0;i<se_a->getNum() - 1; i++)
//    {
//        painter->drawLine(points[i], points[i+1]);
//    }
//}

QPoint Canvas::translateCo(double x, double y)
{
    if (maxX == minX)
    {
        maxX += 5;
        minX -= 5;
    }
    if (minY == maxY)
    {
        minY -= 5;
        maxY += 5;
    }
    double k = double(this->width() - 2*PADDING)/(maxX - minX);
    int x_t = k * x + PADDING - k * minX;

    k = double(2*PADDING - this->height())/(maxY - minY);
    int y_t = k * y + PADDING - k * maxY;
    QPoint res;
    res.setX(x_t);
    res.setY(y_t);
    return res;
}

void Canvas::getRange()
{
    maxX = se_draw[0].getMaxX();
    maxY = se_draw[0].getMaxY();
    minX = se_draw[0].getMinX();
    minY = se_draw[0].getMinY();
    if(se_num == 1)
    {
        return;
    }
    for (int i = 1; i<se_num; i++)
    {
        maxX = se_draw[i].getMaxX() > maxX?se_draw[i].getMaxX(): maxX;
        maxY = se_draw[i].getMaxY() > maxY?se_draw[i].getMaxY(): maxY;
        minX = se_draw[i].getMinX() < minX?se_draw[i].getMinX(): minX;
        minY = se_draw[i].getMinY() < minY?se_draw[i].getMinY(): minY;
    }
    if (minX == maxX)
    {
        minX = minX - 5;
        maxX = maxX + 5;
    }
    if (minY == maxY)
    {
        minY = minY - 5;
        maxY = maxY + 5;
    }
}

void Canvas::drawAxies()
{
    pen->setColor(Qt::black);
    pen->setStyle(Qt::SolidLine);
    pen->setWidth(1);
    painter->setPen(*pen);
    QPoint start;
    QPoint end;
    int width = this->width() - PADDING;
    int height = this->height() - PADDING;
    changeRES(width - PADDING,height- PADDING);
    int w = width/xresolution;
    int h = height/yresolution;


    for (int i = 0; i<yresolution; i++)
    {
        start.setX(PADDING);
        start.setY(i*h);
        end.setX(width);
        end.setY(i*h);
        painter->drawLine(start,end);
    }
    for (int j = 0; j<xresolution; j++)
    {
        start.setX(j*w);
        start.setY(PADDING);
        end.setX(j*w);
        end.setY(height);
        painter->drawLine(start,end);
    }
}

Canvas::Canvas(QWidget *parent) : QWidget(parent)
{
    xresolution = 10;
    yresolution = 10;

    se_draw = NULL;
    se_num = 0;
    resize(600,600);
    setMinimumSize(200,200);
}

void Canvas::setSequences(Sequence *se_a, int n)
{
    se_draw = se_a;
    se_num = n;
    update();
}


void Canvas::paintEvent(QPaintEvent *)
{
    update();
    painter = new QPainter(this);
    painter->setRenderHint(QPainter::Antialiasing, true);
    pen = new QPen;

    pen->setColor(Qt::gray);
    pen->setWidth(1);
    painter->setPen(*pen);

    drawGrid();
    for (int i = 0; i<se_num; i++)
    {
        drawSeq(&se_draw[i],i);
    }
}

/*
*
*
*  ━━━━━━神兽出没━━━━━━
* 　　　┏┓　　　┏┓
* 　　┏┛┻━━━┛┻┓
* 　　┃　　　　　　　┃
* 　　┃　　　━　　　┃
* 　　┃　┳┛　┗┳　┃
* 　　┃　　　　　　　┃
* 　　┃　　　┻　　　┃
* 　　┃　　　　　　　┃
* 　　┗━┓　　　┏━┛Code is far away from bug with the animal protecting
* 　　　　┃　　　┃    神兽保佑,代码无bug
* 　　　　┃　　　┃
* 　　　　┃　　　┗━━━┓
* 　　　　┃　　　　　　　┣┓
* 　　　　┃　　　　　　　┏┛
* 　　　　┗┓┓┏━┳┓┏┛
* 　　　　　┃┫┫　┃┫┫
* 　　　　　┗┻┛　┗┻┛
*
*/
