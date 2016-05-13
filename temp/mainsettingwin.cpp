#include "mainsettingwin.h"
#include "ui_mainsettingwin.h"

mainSettingWin::mainSettingWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainSettingWin)
{
    ui->setupUi(this);
}

mainSettingWin::~mainSettingWin()
{
    delete ui;
}
