#ifndef CLUSTERDEMO_H
#define CLUSTERDEMO_H

#include <QWidget>
typedef struct Line{
    QPoint startPnt;
    QPoint endPnt;
}Line;
class ClusterDemo : public QWidget
{
public:
    explicit ClusterDemo(QWidget *parent = 0);
    void paintEvent(QPaintEvent) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent) Q_DECL_OVERRIDE;
    ~ClusterDemo();
private:
    QPoint startPnt;   //起点
    QPoint endPnt;     //终点
    bool isPressed;    //鼠标是否按下
    std::vector<Line> lines; //存放所有的线段
};

#endif // CLUSTERDEMO_H
