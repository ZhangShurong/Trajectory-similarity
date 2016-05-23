#include "settingwin.h"
#include "ui_mainwindow.h"

SettingWin::SettingWin(Ui::MainWindow *ui, QObject *parent) : QObject(parent)
{
    this->ui = ui;
    settingPath = "./setting.ini";
    setting = new QSettings(settingPath, QSettings::IniFormat);
    initComBox();
    initSettingMap();
    readIni();
}

SettingWin::~SettingWin()
{
    delete ui;
}

void SettingWin::initComBox()
{
    QStringList setValue;
    setValue << "显示时间标签与点"
             << "不显示时间标签"
             << "仅显示轨迹";
    ui->mapModeComboBox->addItems(setValue);
    setValue.clear();
    setValue << "2"
             << "3"
             << "4"
             << "5"
             << "6"
             << "7"
             << "8"
             << "10"
             << "20";
    ui->seqNumComboBox->addItems(setValue);
}

bool SettingWin::readIni()
{
    settingMap["seqLimit"] = setting->value("seqLimit").toInt();
    if (settingMap["seqLimit"] == 0)
    {
        qDebug() << "No value";
        setting->setValue("seqLimit", "10");
    }
    else
    {
        qDebug() << QString::number(settingMap["seqLimit"]);
    }
    return true;
}

void SettingWin::initSettingMap()
{
    settingMap.insert("seqLimit", 0);
}

