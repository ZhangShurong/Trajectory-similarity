#ifndef MAPSETTINGWIN_H
#define MAPSETTINGWIN_H

#include <QMainWindow>

namespace Ui {
class mapSettingWin;
}

class mapSettingWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit mapSettingWin(QWidget *parent = 0);
    ~mapSettingWin();

private:
    Ui::mapSettingWin *ui;
};

#endif // MAPSETTINGWIN_H
