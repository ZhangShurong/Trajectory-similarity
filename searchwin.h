#ifndef SEARCHWIN_H
#define SEARCHWIN_H
#include <QObject>
#include "sequence.h"
#include "mainwindow.h"

#define ROW_NUM 100

namespace Ui {
class MainWindow;
}
class MapWindow;
class MainWindow;
class SearchWin : public QObject
{
    Q_OBJECT
public:
    SearchWin(Ui::MainWindow *ui,DataBase *db);
    void setTracs(QStringList *tracs);
    void setDB(DataBase *db);
    void refreshTable();
    void setNumOfSeqs(int num);
    void init();
    ~SearchWin();
private:
    Ui::MainWindow *ui;
    Sequence input;
    DataBase *db;
    QVector<Sequence> seqs;
    int numOfSeqs;
    bool time;
    int rowcount;
    QStringList *tracs;
    void initTable(QTableWidget *table);
    void initSeqPartTable(QTableWidget *table);
    void initPointTable(QTableWidget *table);
    void showPartofSeq();
    //void search(Sequence input);
    void search();
    void sortPointTable(QTableWidget *table);
    void initSig();
    void fillPointTable(QTableWidget *table, QVector<PointCompare> pointsV, Sequence *se);
    void fillTable(Sequence inSeq);


public slots:
    void openFile();
    void rankPartOfSeq();
    void rankSeqChecked();
    void rankSeqPointChecked();
    void startSearch();
};

#endif // SEARCHWIN_H
