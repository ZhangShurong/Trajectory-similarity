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
    Sequence *input;
    DataBase *db;
    QVector<Sequence> seqs;
    int numOfSeqs;
    bool time;//表示是否有时间
    bool distinct;
    int rowcount;
    int partRowcount;

    QMap<QString, int> seq_index;
    QMap<QString, Sequence> id_seq_map;

    QStringList *tracs;
    QStringList coincide;//专门存储轨迹id

    void initTable(QTableWidget *table);
    void initSeqPartTable(QTableWidget *table);
    void initPointTable(QTableWidget *table);
    void showPartofSeq();
    void drawPoints();

    //void search();
    void searchSeq();
    void sortPointTable();
    void sortPartTable();
    void sortSeqTable();
    void initSig();
    void fillPointTable(QTableWidget *table, QVector<PointCompare> pointsV, Sequence *se);
    void fillPartTable(QTableWidget *table, QVector< QVector<int> > partInfo, Sequence *se);
    void fillTable(Sequence inSeq);
    void calSecPart();

    void drawSeq();

    void searchPoint();
    void loadIntoMem();

public slots:
    void openFile();
    void rankPartOfSeq();
    void rankSeqClicked();
    void rankSeqPointClicked();
};

#endif // SEARCHWIN_H
