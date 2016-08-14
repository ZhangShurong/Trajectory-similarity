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
private:
    Ui::MainWindow *ui;
    Client *client;
    void upload(vector<Sequence> sequences);
};

#endif // CLOUD_H
