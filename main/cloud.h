#ifndef CLOUD_H
#define CLOUD_H

#include <QMainWindow>
#include "header.h"
#include "sequence.h"
#include "client.h"
#include "database.h"
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
    void downloadPushBtnClicked();

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
    bool connected;

    void connectServer();
    void disconnectServer();

    void initTable();
    void display(vector<Sequence> sequences);
    void displayRes(vector<Result> resSet);

    vector<Sequence> getSequences();
    vector<Result> getResSet();

    void download(vector<Sequence> seqs);
};

#endif // CLOUD_H
