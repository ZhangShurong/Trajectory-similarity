#include "settingwin.h"
#include "ui_mainwindow.h"

settingWin::settingWin(QObject *parent) : QObject(parent)
{
    settingPath = "./setting.ini";
    setting = new QSettings(settingPath, QSettings::IniFormat);
}

settingWin::~settingWin()
{
    delete ui;
}

