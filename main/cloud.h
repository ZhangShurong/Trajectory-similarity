#ifndef CLOUD_H
#define CLOUD_H

#include <QMainWindow>
#include "header.h"
#include "sequence.h"
#include "client.h"
namespace Ui {
class MainWindow;
}

class Cloud : public QMainWindow
{
    Q_OBJECT
public:
   explicit Cloud(Ui::MainWindow *ui,QWidget *parent = 0);
    ~Cloud();
private slots:
    void openfiles();
    void openfile();//For search
    void refreshBtnClicked();
    void connectPushBtnClicked();

    //for socket
    void connectedMsg();
    void stop();
    void connectionClosedByServer();
    void error();
    void readData();
private:
    Ui::MainWindow *ui;
    Client *client;
    QString ipAdd;
    uint portd;

    void connectServer();
    void disconnectServer();

    void initTable();
    void display(vector<Sequence> sequences);

    vector<Sequence> getSequences();

};

#endif // CLOUD_H
