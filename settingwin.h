#ifndef SETTINGWIN_H
#define SETTINGWIN_H

#include <QObject>
#include <QSettings>

namespace Ui {
class MainWindow;
}
class settingWin : public QObject
{
    Q_OBJECT
public:
    explicit settingWin(QObject *parent = 0);
    ~settingWin();
private:
    Ui::MainWindow *ui;
    QSettings *setting;
    QString settingPath;
signals:

public slots:
};

#endif // SETTINGWIN_H
