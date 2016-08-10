#include "omp.h"
#include "database.h"
#include <QtCore/QCoreApplication>
#include <QtSql>
#include "core.h"

DataBase::DataBase()
{
    dbName = "test.db";
    loadDriver();
    initDb();
}

DataBase::DataBase(int /*i*/)
{

}

DataBase::DataBase(string databaseName)
{
    dbName = databaseName;
    loadDriver();
    initDb();
}

void DataBase::clearDB(string tableName)
{
    QSqlQuery q;
    QString str = "DELETE FROM "
                  + QString::fromStdString(tableName)
                  + ";";
    q.exec(str);
}

void DataBase::setDBName(string databaseName)
{
    dbName = databaseName;
}

bool DataBase::loadDriver()
{
    if (!QSqlDatabase::drivers().contains("QSQLITE"))
    {
        fprintf(stderr,"Unable to load database\n");
        return false;
    }
    fprintf(stdout,"Driver loaded\n");
    return false;
}

QSqlError DataBase::initDb()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QString::fromStdString(dbName));

    if (!db.open()) {
        fprintf(stderr,"Can not open database %s \n", dbName.c_str());
        return db.lastError();
    }
    qDebug() << "Connected!";
    return QSqlError();
}

void DataBase::decodeID(string ID, int *start, int *end)
{
    QStringList list = QString::fromStdString(ID).split("_");
    QString s = list[0];
    QString e = list[1];
    *start = s.toInt();
    *end = e.toInt();
}


bool DataBase::isTableExist(string tableName)
{
    QSqlQuery query;
    bool exist;
    query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1';").arg(QString::fromStdString(tableName)));
    query.next();
    exist = query.value(0).toBool();
    return exist;
}

string DataBase::insertData(Csv *csv, string tbName)
{
    QSqlQuery q;
    QString insert;
    QString tableName = QString::fromStdString(tbName);

//    db.transaction();

    if (!isTableExist(tbName))
    {
        createTable(tbName);
    }
    QSqlQuery query;
    QString start;
    QString end;

    if (getRecordNum(tableName.toStdString()) == 0)
    {
        query.exec("insert into " + tableName
                   + " values('',NULL,NULL,NULL);");
    }
    query.exec("select max(id) from "+tableName+";");
    if(query.next()) {
        start = query.value(0).toString();
    }


    Sequence *temp = new Sequence();
    getSquFromFile(csv,temp);
    if(!temp->hasTime())
    {
        for (int i = 0; i < temp->pointsNum; i++)
        {
            insert = "insert into " + tableName +
                     " values ('','POINT(" +
                    QString::number(temp->pts[i].longitude) + " " +
                    QString::number(temp->pts[i].latitude) + " " +
                    ")',NULL,NULL,NULL);";
            q.exec(insert);
        }
    }
    else
    {
        for (int i = 0; i < temp->pointsNum; i++)
        {
            insert = "insert into " + tableName +
                     " values ('','POINT(" +
                    QString::number(temp->pts[i].longitude) + " " +
                    QString::number(temp->pts[i].latitude) + " " +
                    ")','"+
                     temp->pts->time+
                    "',NULL,NULL);";

            q.exec(insert);
        }
    }
    query.exec("select max(id) from "+tableName);
    while (query.next()) {
        end = query.value(0).toString();
    }
    //------------------------//
/*
    double minLongtitude = 73;
    double maxLongtitude = 136;
    double minLatitude = 3;
    double maxLatitude = 54;
    QString tid = (start+"_"+end);
    insert = "insert into "
             + tableName
             + " values ("
             + "'"
             + tid
             + "'"
             + ",NULL,NULL,NULL,"
             + QString::number(hardCluster(temp, minLongtitude,
                           maxLongtitude,
                           minLatitude,
                           maxLatitude,
                           6
                           ))
             + ");";
*/
    QString tid = (start+"_"+end);
    insert = "insert into "
             + tableName
             + " values ("
             + "'"
             + tid
             + "'"
             + ",NULL,NULL,NULL,"
             + temp->getType()
             + ");";
    query.exec(insert);
 //   db.commit();
    return tid.toStdString();

/*
    while (csv->getline(line) != 0) {
        if (line.empty())
        {
            continue;
        }

        const char *t = line.c_str();
        if (Csv::notStdAscii(t[0]))
            continue;
        insert = "insert into " + tableName +
                 " values ('','POINT(";
        int j = csv->getnfield();
        if (j == 2)
        {
            for (int i = 0; i < j; i++)
            {
                insert += QString::fromStdString(csv->getfield(i)) + " ";
                //qDebug() << QString::fromStdString(csv->getfield(i)) << "\n";
            }
            insert += ")',NULL,NULL);";

        }
        else if (j==3)
        {
            for (int i = 0; i < 2; i++)
            {
                insert += QString::fromStdString(csv->getfield(i)) + " ";
            }
            string s="/";
            if(strstr(csv->getfield(2).c_str(),s.c_str())){
                throw -1;
                return "Error";
            }
            insert = insert + ")','"
                     + QString::fromStdString(csv->getfield(2))
                     +"',NULL);";
        }
        else
        {
            fprintf(stderr, "Format wrong\n");
            return NULL;
        }
            q.exec(insert);
    }
    query.exec("select max(id) from "+tableName);
    while (query.next()) {
        end = query.value(0).toString();
    }
    //------------------------//
    QString tid = (start+"_"+end);
    insert = "insert into "
             + tableName
             + " values ("
             + "'"
             + tid
             + "'"
             + ",NULL,NULL,NULL);";

    //------------------------//
    query.exec(insert);
 //   db.commit();
    return tid.toStdString();
   */
}


void DataBase::getSequenceByID(string tableName, Sequence *squ, string ID)
{

    int start,end;
    decodeID(ID, &start, &end);
    int l = end - start;
    if (l == 0)
    {
        return;
    }
    Point *temp = new Point[l];
    QSqlQuery query;
    query.exec("SELECT pt, time, id from "
               + QString::fromStdString(tableName)
               +" where id between "
               + QString::number(start + 1)
               + " and "
               + QString::number(end)
               + ";"
              );
    query.next();
    for (int i = 0; i< l; i++)
    {
        QString pt = query.value(0).toString();
        QString time = query.value(1).toString();
        QString id = query.value(2).toString();
        temp[i].buildPoint(pt,time,id);
        query.next();
        // qDebug() << pt + time + id;
    }
    squ->setPtNum(l);
    squ->setPoints(temp);
    squ->setID(QString::fromStdString(ID));
    query.exec("SELECT type from "
               + QString::fromStdString(tableName)
               +" where tid = '"
               +QString::fromStdString(ID)
               +"';"
              );
    query.next();
    QString type = query.value(0).toString();
    squ->setType(type);
    //qDebug() << squ->getNum();
}

int DataBase::getPointNumByID(string ID)
{
    int a,b;
    decodeID(ID, &a, &b);
    return b-a;
}

QStringList *DataBase::getAllTracID(string tableName)
{
    QStringList *temp = new QStringList();
    QString str = "select tid from "
                  + QString::fromStdString(tableName)
                  + " where tid <> '';";
    QSqlQuery q;
    q.exec(str);
    while (q.next()) {
        *temp << q.value(0).toString();
    }
    return temp;
}

int DataBase::getRecordNum(string tableName)
{
    int result = 0;
    QString str = "select count(*) from "
                  + QString::fromStdString(tableName)
                  + ";";
    QSqlQuery q;
    q.exec(str);
    while (q.next()) {
        result = q.value(0).toString().toInt();
    }
    return result;
}

void DataBase::createTable(string tableName)
{
    QSqlQuery query;
    query.exec("create table "+QString::fromStdString(tableName)+"(tid varchar,pt varchar,time datetime,id integer primary key, type int)");
}

Sequence * DataBase::getNSequences(int &n, string tableName)
{
    QStringList *allTascID = getAllTracID(tableName);
    int num = allTascID->length();
    string id;
    Sequence *all;
    if (n<0 || n >num)
    {
        all = new Sequence[num];
        n = num;
    }
    else
        all = new Sequence[n];
    for (int i = 0; i<n; i++)
    {

        id = QString((*allTascID)[i]).toStdString();
        getSequenceByID(tableName,&all[i],id);
    }
    return all;
}

bool DataBase::hasTime(string ID,string tableName)
{
    int start,end;
    decodeID(ID, &start, &end);

    QSqlQuery query;
    query.exec("SELECT pt, time, id from "
               + QString::fromStdString(tableName)
               +" where id between "
               + QString::number(start + 1)
               + " and "
               + QString::number(end)
               + ";"
              );
    query.next();
    QString time = query.value(1).toString();
    query.clear();
    return (time != "");
}

void DataBase::delSeq(string ID,string tableName)
{
    //delete from importtest where tid =
    int start,end;
    decodeID(ID, &start, &end);
    QSqlQuery query;
    query.exec("DELETE from "
               + QString::fromStdString(tableName)
               +" where id between "
               + QString::number(start + 1)
               + " and "
               + QString::number(end + 1)
               + ";"
              );
}
