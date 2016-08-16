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

DataBase::DataBase(QString connName, string databaseName)
{
    dbName = databaseName;
    loadDriver();
    //initDb();
    if(connName.isEmpty())
        connName=QSqlDatabase::defaultConnection;
    if(QSqlDatabase::contains(connName)){ //if connection is contained
        QSqlDatabase tmpdb=QSqlDatabase::database(connName);
        tmpdb.close();
    }
    //QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL",connName);

    db = QSqlDatabase::addDatabase("QSQLITE",connName);
    db.setDatabaseName(QString::fromStdString(dbName));

    if (!db.open()) {
        fprintf(stderr,"Can not open database %s \n", dbName.c_str());
        //return db.lastError();
    }
    qDebug() << "Connected! And connName is " << connName;
}

void DataBase::clearDB(string tableName)
{
    QSqlQuery q(db);
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
    //QSqlQuery query;
    QSqlQuery query(db);
    bool exist;
    query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1';").arg(QString::fromStdString(tableName)));
    query.next();
    exist = query.value(0).toBool();
    return exist;
}

string DataBase::insertData(Csv *csv, string tbName)
{
    QSqlQuery q(db);
    QString insert;
    QString tableName = QString::fromStdString(tbName);

    if (!isTableExist(tbName))
    {
        createTable(tbName);
    }
    //QSqlQuery query;
    QSqlQuery query(db);
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
    char  buffer1[200];
    char  buffer2[200];
    QString str1;
    QString str2;
    if(!temp->hasTime())
    {
        for (int i = 0; i < temp->pointsNum; i++)
        {

            sprintf(buffer1,"%.6lf",temp->pts[i].longitude);
            sprintf(buffer2,"%.6lf",temp->pts[i].latitude);
            str1 = QString(QLatin1String(buffer1));
            str2 = QString(QLatin1String(buffer2));
            insert = "insert into " + tableName +
                    " values ('','POINT(" +
                    str1 + " " +
                    str2 + " " +
                    ")',NULL,NULL,NULL);";
            q.exec(insert);
        }
    }
    else
    {
        for (int i = 0; i < temp->pointsNum; i++)
        {
            sprintf(buffer1,"%.6lf",temp->pts[i].longitude);
            sprintf(buffer2,"%.6lf",temp->pts[i].latitude);
            str1 = QString(QLatin1String(buffer1));
            str2 = QString(QLatin1String(buffer2));
            insert = "insert into " + tableName +
                    " values ('','POINT(" +
                    str1 + " " +
                    str2 + " " +
                    ")','"+
                    temp->pts[i].time+
                    "',NULL,NULL);";

            q.exec(insert);
        }
    }
    query.exec("select max(id) from "+tableName);
    while (query.next()) {
        end = query.value(0).toString();
    }

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
    return tid.toStdString();
}

string DataBase::insertData(Sequence sequence, string tbName)
{
    QString insert;
    QString tableName = QString::fromStdString(tbName);
    if (!isTableExist(tbName))
    {
        createTable(tbName);
    }
    //QSqlQuery query;
    QSqlQuery query(db);
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

    char  buffer1[200];
    char  buffer2[200];
    QString str1;
    QString str2;
    if(!sequence.hasTime())
    {
        for (int i = 0; i < sequence.pointsNum; i++)
        {

            sprintf(buffer1,"%.6lf",sequence.pts[i].longitude);
            sprintf(buffer2,"%.6lf",sequence.pts[i].latitude);
            str1 = QString(QLatin1String(buffer1));
            str2 = QString(QLatin1String(buffer2));
            insert = "insert into " + tableName +
                    " values ('','POINT(" +
                    str1 + " " +
                    str2 + " " +
                    ")',NULL,NULL,NULL);";
            query.exec(insert);
        }
    }
    else
    {
        for (int i = 0; i < sequence.pointsNum; i++)
        {
            sprintf(buffer1,"%.6lf",sequence.pts[i].longitude);
            sprintf(buffer2,"%.6lf",sequence.pts[i].latitude);
            str1 = QString(QLatin1String(buffer1));
            str2 = QString(QLatin1String(buffer2));
            insert = "insert into " + tableName +
                    " values ('','POINT(" +
                    str1 + " " +
                    str2 + " " +
                    ")','"+
                    sequence.pts[i].time+
                    "',NULL,NULL);";

            query.exec(insert);
        }
    }
    query.exec("select max(id) from "+tableName);
    while (query.next()) {
        end = query.value(0).toString();
    }

    QString tid = (start+"_"+end);
    insert = "insert into "
            + tableName
            + " values ("
            + "'"
            + tid
            + "'"
            + ",NULL,NULL,NULL,"
            + sequence.getType()
            + ");";
    query.exec(insert);
    return tid.toStdString();
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
    //QSqlQuery query;
    QSqlQuery query(db);
    query.exec("SELECT pt, time, id from "
               + QString::fromStdString(tableName)
               +" where id between "
               + QString::number(start + 1)
               + " and "
               + QString::number(end)
               + ";"
               );
    query.next();
    QString pt;
    QString time;
    QString id ;
    for (int i = 0; i< l; i++)
    {
        pt = query.value(0).toString();
        time = query.value(1).toString();
        id = query.value(2).toString();
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
    QStringList *temp = new QStringList;
    QString str = "select tid from "
            + QString::fromStdString(tableName)
            + " where tid <> '';";
    QSqlQuery q(db);
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
    //QSqlQuery q;
    QSqlQuery q(db);
    q.exec(str);
    while (q.next()) {
        result = q.value(0).toString().toInt();
    }
    return result;
}

void DataBase::createTable(string tableName)
{
    //QSqlQuery query;
    QSqlQuery query(db);
    query.exec("create table "+QString::fromStdString(tableName)+"(tid varchar,pt varchar,time datetime,id integer primary key, type int)");
}

vector<Sequence> DataBase::getNSequences(int n, string tableName)
{
    //loadinto memory
    QSqlQuery query(db);
    //select id from Server  where tid <> "" limit 10;
    QString qstr = "select id from " +QString::fromStdString(tableName) + "where tid <> '' limit " + n;
    query.exec(qstr);
    query.next();
    query.clear();
    QString maxid = query.value(0).toString();
    qstr = "select * from Server  where id <= " + maxid;
    //tid,pt,time,id,type
    query.exec(qstr);
    QString tid;
    QString pt;
    QString time;
    QString id ;

    Point temp;
    Sequence t;
    vector<Sequence>  res;
    while (query.next()) {
        tid = query.value(0).toString();
        if(tid.isEmpty())
        {
            pt = query.value(1).toString();
            time = query.value(2).toString();
            id = query.value(3).toString();
            temp.buildPoint(pt,time,id);
            t.appendPt(&temp);
        }
        else
        {
            t.setID(tid);
            res.push_back(t);
            t.clear();
        }
        query.next();
        // qDebug() << pt + time + id;
    }
    return res;
}

bool DataBase::hasTime(string ID,string tableName)
{
    int start,end;
    decodeID(ID, &start, &end);
    //QSqlQuery query;
    QSqlQuery query(db);
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
    QSqlQuery query(db);
    query.exec("DELETE from "
               + QString::fromStdString(tableName)
               +" where id between "
               + QString::number(start + 1)
               + " and "
               + QString::number(end + 1)
               + ";"
               );
}

void DataBase::close()
{
    db.close();
}
void DataBase::closeConnection(QString connName){
    if(connName.isEmpty())
        connName=QSqlDatabase::defaultConnection;
    if(!QSqlDatabase::contains(connName))
        return;
    {//force db to destruct before removeDatabase
        QSqlDatabase db=QSqlDatabase::database(connName);
        if(db.isOpen()){
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(connName);
}
