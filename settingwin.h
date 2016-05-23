#ifndef SETTINGWIN_H
#define SETTINGWIN_H

#include <QObject>
#include <QSettings>


namespace Ui {
class MainWindow;
}

class SettingWin : public QObject
{
    Q_OBJECT
public:
    explicit SettingWin(Ui::MainWindow *ui, QObject *parent = 0);
    ~SettingWin();
private:
    Ui::MainWindow *ui;
    QSettings *setting;
    QString settingPath;
    QMap<QString, int> settingMap;
    //int seqLimit;
    void initComBox();
    bool readIni();
    void initSettingMap();
signals:

public slots:
};

#endif // SETTINGWIN_H
