#ifndef CANVAS_H
#define CANVAS_H
#include "core.h"
#include <QWidget>
#include <vector>
#include <QEvent>
#include <QMouseEvent>


class Canvas : public QWidget
{
    Q_OBJECT
private:
    double minX;
    double minY;
    double maxX;
    double maxY;
    //many sequences
    Sequence *se_draw;
    int se_num;

    QPainter *painter;
    QPen *pen;
    int xresolution;
    int yresolution;
    void drawGrid();
    void changeRES(int width,int height);
    QPoint translateCo(double x, double y);
    void drawAxies();
    void getRange();
    void drawSeq(Sequence *se_a);

public:
    explicit Canvas(QWidget *parent = 0);
    void setSequences(Sequence *se_a,int n);
    //void drawSeq(Sequence *se_a, QColor c, int weight);
protected:
    void paintEvent(QPaintEvent *event);
signals:

public slots:
};

#endif // CANVAS_H
