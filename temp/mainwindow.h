#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "header.h"
#include "csv.h"
#include "database.h"
#include "mapwindow.h"
#include "canvas.h"
#include "calwindow.h"


class MapWindow;
class calWindow;


class mainWindow : public QWidget
{
    Q_OBJECT
    string tName;

    QHBoxLayout *mainLayout;

    QTableWidget *mainTable;

    QMenuBar *menuBar;

    QMenu *fileMenu;
    QAction *openAct;
    QAction *importFiles;
    QAction *exportOne;
    QAction *exportAll;

    QMenu *mapMenu;
    QAction *showMapAct;
    QAction *paintAct;

    QMenu *tracMenu;
    QAction *searchAct;

    QMenu *dbMenu;
    QAction *clearAct;
    QAction *showAct;

    QMenu *calMenu;
    QAction *calAct;
//left Part
    QWidget *left;
    QVBoxLayout *leftLayout;
    QPushButton *openFileButton;
    QPushButton *calButton;

    DataBase *db;

    MapWindow *map;
    Canvas *can;
//CSS
    QFile *cssFile;
    QString *strCSS;

    calWindow *calWin;

public:
    explicit mainWindow(QWidget *parent = 0);
    void setDb(DataBase *database);
    void loadFile();

private:
    void initCSS();
    void initLeft();
    void createActions();
    void createMenus();
    void initTable();
    void refreshTable();


signals:

public slots:
    void openFile();
    void showMap();
    void about();
    void searchInDB();
    void paint();
    void clearDB();
    void calActSlot();
    void openFilesSlot();
};

#endif // MAINWINDOW_H
