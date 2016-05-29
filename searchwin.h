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

//    QMap<int, int> p2p;//存贮输入轨迹与数据库轨迹的

    void initTable(QTableWidget *table);
    void initSeqPartTable(QTableWidget *table);
    void initPointTable(QTableWidget *table);
    void showPartofSeq();
    void drawPoints();
    //void search(Sequence input);
    void search();
    void sortPointTable(QTableWidget *table);
    void initSig();
    void fillPointTable(QTableWidget *table, QVector<PointCompare> pointsV, Sequence *se);
    void fillPartTable(QTableWidget *table, QVector< QVector<int> > partInfo, int beginMin1, int beginMin2, Sequence *se);
    void fillTable(Sequence inSeq);


public slots:
    void openFile();
    void rankPartOfSeq();
    void rankSeqChecked();
    void rankSeqPointChecked();
    void startSearch();
};

#endif // SEARCHWIN_H
