#ifndef CLOUD_H
#define CLOUD_H

#include <QMainWindow>
#include "header.h"
#include "sequence.h"
namespace Ui {
class Cloud;
}

class Cloud : public QMainWindow
{
    Q_OBJECT
public:
   explicit Cloud(QWidget *parent = 0);
private:
    Ui::Cloud *ui;
};

#endif // CLOUD_H
