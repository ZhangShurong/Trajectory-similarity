/*
 @header     canvas.h

 @author     柯磊

 @abstract   用于处理计算模块，ui作为参数从mainwindow传入
*/




#ifndef CALWINDOW_H
#define CALWINDOW_H
#include <QMainWindow>
#include "header.h"
#include "sequence.h"
#include <QEvent>
#include <QCloseEvent>
#include <string>
#include "core.h"
#include "partdetail.h"
#include "pointdetail.h"

namespace Ui {
class MainWindow;
}
class MapWindow;

class CalWindow : public QObject
{
    Q_OBJECT
public:
    CalWindow(Ui::MainWindow *ui);
    ~CalWindow();
private:
    Ui::MainWindow *ui;
    Sequence *p;
    Sequence *q;
    QString namep;
    QString nameq;
    partDetail pd;
    PointDetail pointDetail;
    void calSeq();
    void calPoint();


protected:

public slots:
    void openFile1();
    void openFile2();
    void startSlot();
    void showPartDetail();
    void showPointDetail();
};

#endif // CALWINDOW_H
