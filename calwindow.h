#ifndef CALWINDOW_H
#define CALWINDOW_H
#include <QMainWindow>
#include "header.h"
#include "sequence.h"
#include <QEvent>
#include <QCloseEvent>
#include <string>
#include "core.h"

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
    void calSeq();
    void calPoint();

protected:

public slots:
    void openFile1();
    void openFile2();
    void startSlot();
};

#endif // CALWINDOW_H
