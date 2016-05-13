#ifndef MAPWIN_H
#define MAPWIN_H

#include <QMainWindow>

namespace Ui {
class MapWin;
}

class MapWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit MapWin(QWidget *parent = 0);
    ~MapWin();
private:
    Ui::MapWin *ui;
};

#endif // MAPWIN_H
