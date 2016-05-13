#ifndef CALPANEL_H
#define CALPANEL_H

#include <QMainWindow>
#include "header.h"
#include "sequence.h"
#include "mapsettingwin.h"
#include <QEvent>
#include <QCloseEvent>
#include <string>
#include "core.h"

using namespace std;
//namespace Ui {
//class calPanel;
//}
namespace Ui {
class calPanel;
}
//自制画图面板，以便无网络时使用
class calPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit calPanel(QWidget *parent = 0);
    ~calPanel();

private:
    Ui::calPanel *ui;
    Sequence *p;
    Sequence *q;
    mapSettingWin *setWin;
protected:
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);


public slots:
    void openFile1();
    void openFile2();
    void startSlot();
    void settingSlot();
};

#endif // CALPANEL_H
