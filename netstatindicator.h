#ifndef NETSTATINDICATOR_H
#define NETSTATINDICATOR_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QUrl>
#include <QTcpSocket>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QTimer>

class NetStatIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit NetStatIndicator(QWidget *parent = 0);
private:
    static const int timeout = 2 * 1000; //ms
    static const int interval = 6 * 1000; //ms
    bool   alerted;
    double netspeed;
    QTimer timer;
    QLabel *netStatLabel;
    QLabel *netspeedLabel;

    static QString toHumanSpeed(double speed);
    static QString toDescription(double speed);
signals:

public slots:
    void updateNetStat();
    void updateUi();
    void alertProperly();
};

#endif // NETSTATINDICATOR_H
