#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include "header.h"
#include "csv.h"
#include "database.h"
#include "mapwindow.h"
#include "canvas.h"
#include "calpanel.h"
#include "detailwin.h"
#include "mainsettingwin.h"

//#include "calwindow.h"
class MapWindow;
class calWindow;

namespace Ui {
class mainWin;
}

class mainWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainWin(QWidget *parent = 0);
    void setDb(DataBase *database);
    QVector<Sequence> getCurrentSeqs();
    ~mainWin();
protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::mainWin *ui;
    string tName;
    bool search_mode;
    DataBase *db;
    MapWindow *map;
    Canvas *can;
  //CSS
    QFile *cssFile;
    QString *strCSS;
   // calWindow *calWin;
    calPanel *calWin;
    detailWin *detWin;
    mainSettingWin *setWin;

    QVector<Sequence> selectedSeqS;

//Table action
    QAction *deleteAct;
    QAction *searchAct;
    QAction *showInMapAct;
    QAction *detailAct;

    Sequence input;


    void initAction();
    void initCSS();
    void initTable();
    void refreshTable();
    void initSig();
    void initCan();
    void search(Sequence input);
    void initMap();
    void searchMode(bool inSearch);
    void getDetail(Sequence *se_a);
public slots:
    void openFile();
    void showMap();
    void about();
    void searchInDB();
    void paint();
    void clearDB();
    void calActSlot();
    void openFilesSlot();
    void clickTBSlot(const QModelIndex index);
    void deleteOneAct();
    void searchSlot_R();
    void showInMapSlot_R();
    void detailSlot_R();
    void goBackSlot();
    void showInMap();
    void setSlot();
};


#endif // MAINWIN_H
