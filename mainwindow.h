#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "header.h"
#include "csv.h"
#include "database.h"
#include "mapwindow.h"
#include "canvas.h"
#include "detailwin.h"
#include "calwindow.h"
#include "searchwin.h"
#include "settingwin.h"

#define ROW_NUM 100

#define ANIMATE_DURATION 400

class MapWindow;
class CalWindow;
class SearchWin;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void setDb(DataBase *database);
    QVector<Sequence> getCurrentSeqs();

    ~MainWindow();

    /*
     * 动画效果
     */
    void startAnimation();
    void closeWindowAnimation();
    void shakeWindow();

    /*
     * starck设置当前页面
     */
    void setStackCurrentPage(quint8 index);


protected:
    /*
     * 窗体拖动
     */
    void mousePressEvent(QMouseEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);

private slots:
    void on_closeToolButton_clicked();
    void on_minToolButton_clicked();
    void on_searchToolButton_clicked();
    void on_checkToolButton_2_clicked();
    void on_calToolButton_clicked();
    void on_mapToolButton_clicked();
    void on_opToolButton_clicked();
    void on_exToolButton_clicked();
    void aboutButton_clicked();
    void on_gjToolButton_clicked();
    void openFile();
    void showMap();
    void about();
    void searchInDB();
    void paint();
    void clearDB();
    void openFilesSlot();
    void clickTBSlot(const QModelIndex index);
    void deleteOneAct();
    void searchSlot_R();
    void showInMapSlot_R();
    void detailSlot_R();
    void goBackSlot();
    void showInMap();
    void setSlot();
    void exportSlot();

private:
    Ui::MainWindow *ui;
    friend class CalWindow;
    friend class SearchWin;
    friend class ImportFilesRunnable;
    SettingWin *settingWin;
    /*
     * 记录窗口位置
     */
    QPoint startPos;
    QPoint endPos;

    string tName;
    bool search_mode;
    DataBase *db;
    Canvas *can;
    //CSS
    QFile *cssFile;
    QString *strCSS;
    // calWindow *calWin;
    detailWin *detWin;
    CalWindow *cal;
    SearchWin *searchWin;
    QStringList *tracs;

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
    void initSearchWin();
    void search(Sequence input);
    void initMap();
    void searchMode(bool inSearch);
    void getDetail(Sequence *se_a);
};

class ImportFilesRunnable : public QRunnable {
private:
    DataBase &database;
    const std::string tName;
    const QStringList fileList;
    MainWindow *const win;
public:
    ImportFilesRunnable(MainWindow *w, DataBase &db, const std::string &t,
                        const QStringList &ls)
            : QRunnable(), database(db), tName(t), fileList(ls), win(w) {}

    void run() {
        foreach (const QString eachFile, fileList) {
            string fileName = eachFile.toLocal8Bit().data();
            string trcID;
            ifstream fin(fileName.c_str());
            try {
                ifstream fin2(fileName.c_str());
                Csv format(fin2);
                Sequence *t = new Sequence();
                getSquFromFile(&format, t);
            } catch(int i) {
                QMessageBox::information(NULL, "Error 错误代码" + QString::number(i),
                                         "时间格式错误,格式类似20160601 13:00:11",
                                         QMessageBox::Yes, QMessageBox::Yes);
                break;
            }
            Csv csv(fin);
            trcID = database.insertData(&csv, tName);
            win->refreshTable();
        }
        QMessageBox::information(win, QObject::tr("完成"),
                                 QObject::tr("您导入的") + QString::number(fileList.size()) +
                                 QObject::tr("个文件处理完毕。"));
    }
};

#endif // MAINWINDOW_H
