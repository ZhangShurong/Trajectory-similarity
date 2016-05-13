#include "mapsettingwin.h"
#include "ui_mapsettingwin.h"

mapSettingWin::mapSettingWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mapSettingWin)
{
    ui->setupUi(this);
}

mapSettingWin::~mapSettingWin()
{
    delete ui;
}
