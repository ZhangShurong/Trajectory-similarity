/**
  *   赛题：第五届“中国软件杯”大学生软件设计大赛——针对以经纬度或经纬度带时间定义的不同轨迹
  *   软件名称：基于“弗雷歇距离”的轨迹相似度分析软件
  *   开发团队：Kryptonite
  *   开发时间：2016年4至6月
  *   文件功能概述：数据库的操作文件
  *                         调用构造函数时会初始化数据库连接；
  *             注意：
  *              该类提供两种插入数据的方式
  *             string insertData(Csv *csv, string tbName);
  *              string insertData(Sequence *seq_a, string tbName);
  */

#ifndef DATABASE_H
#define DATABASE_H
#include "csv.h"
#include "sequence.h"
#include "header.h"
struct Result
{
    QString id;
    int ptNum;
    int time;
    double res;
    QString demo;
};
class DataBase
{
private:
    string dbName;
    bool loadDriver();
    QSqlError initDb();
    void decodeID(string ID, int *start, int *end);
    void close();
public:
    QSqlDatabase db;
    DataBase();
    DataBase(int i);
    DataBase(string databaseName);
    DataBase(QString connName, string databaseName);
    void clearDB(string tableName);
    void setDBName(string databaseName);
    bool isTableExist(string tableName);
    string insertData(Csv *csv, string tbName);
    string insertData(Sequence sequence, string tbName);
    //   string insertData(Sequence *seq_a, string tbName);
    void getSequenceByID(string tableName, Sequence *squ, string ID);
    int getPointNumByID(string ID);
    QStringList *getAllTracID(string tableName);
    int getRecordNum(string tableName);
    int getDimension(string tableName, string ID);
    void createTable(string tableName);
    vector<Sequence> getNSequences(int n, string tableName);
    bool hasTime(string ID, string tableName);
    void delSeq(string ID,string tableName);
    void closeConnection(QString connName="");
    void getNSequences(QMap<QString, Sequence> &id_seq,int n, string tableName);
    QVector<Sequence> getAllSequences(string tableName);
    void createResTable();
    void insertIntoResTable(int n,vector<Sequence> sequences, double *res,bool clear = true);
    vector<Result> getresult();
    void clearRes();
    int getResNum();
};

#endif // DATABASE_H
