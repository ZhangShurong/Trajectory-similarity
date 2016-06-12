#ifndef DATABASE_H
#define DATABASE_H
#include "csv.h"
#include "sequence.h"
#include "header.h"


class DataBase
{
private:
    QSqlDatabase db;
    string dbName;
    bool loadDriver();
    QSqlError initDb();
    void decodeID(string ID, int *start, int *end);
public:
    DataBase();
    DataBase(int i);
    DataBase(string databaseName);
    void clearDB(string tableName);
    void setDBName(string databaseName);
    bool isTableExist(string tableName);
    string insertData(Csv *csv, string tbName);
    string insertData(Sequence *seq_a, string tbName);
    void getSequenceByID(string tableName, Sequence *squ, string ID);
    int getPointNumByID(string ID);
    QStringList* getAllTracID(string tableName);
    int getRecordNum(string tableName);
    int getDimension(string tableName, string ID);
    void createTable(string tableName);
    Sequence *getNSequences(int &n, string tableName);
    bool hasTime(string ID, string tableName);
    void delSeq(string ID,string tableName);
};

#endif // DATABASE_H
