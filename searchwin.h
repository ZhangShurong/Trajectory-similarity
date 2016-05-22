#ifndef SEARCHWIN_H
#define SEARCHWIN_H
#include <QObject>
#include "sequence.h"
#include "mainwindow.h"
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
    ~SearchWin();
private:
    Ui::MainWindow *ui;
    DataBase *db;
    QVector<Sequence> seqs;
    int numOfSeqs;
    bool time;
    void initTable(QTableWidget *table);
    void initSeqPartTable(QTableWidget *table);
    void showPartofSeq();
    void search(Sequence input);
    void initSig();    
    QStringList *tracs;

public slots:
    void openFile();
    void rankPartOfSeq();
    void rankSeqChecked();
    void rankSeqPointChecked();
};

#endif // SEARCHWIN_H
