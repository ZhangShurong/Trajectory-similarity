#ifndef MAINSETTINGWIN_H
#define MAINSETTINGWIN_H

#include <QMainWindow>

namespace Ui {
class mainSettingWin;
}

class mainSettingWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainSettingWin(QWidget *parent = 0);
    ~mainSettingWin();

private:
    Ui::mainSettingWin *ui;
};

#endif // MAINSETTINGWIN_H
