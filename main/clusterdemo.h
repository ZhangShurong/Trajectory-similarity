#ifndef CLUSTERDEMO_H
#define CLUSTERDEMO_H

#include <QWidget>
#include "sequence.h"
#include "core.h"

typedef struct Line{
    QPoint startPnt;
    QPoint endPnt;
}Line;
class ClusterDemo : public QWidget
{
public:
    explicit ClusterDemo(QWidget *parent = 0);
    void paintEvent(QPaintEvent *p) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent  *e) Q_DECL_OVERRIDE;
    bool clustered;
    ~ClusterDemo();
private:
    Sequence temp;
    std::vector<Sequence> sequences;
    std::vector<QPoint> points;
    int index;
    int pointsNum;
    QPoint startPnt;   //起点
    QPoint endPnt;     //终点
    bool isPressed;    //鼠标是否按下
    std::vector<Line> lines; //存放所有的线段
    std::vector<int> res;
    void reset();
    void cluster();
};

#endif // CLUSTERDEMO_H
