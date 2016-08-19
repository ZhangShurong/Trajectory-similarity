#include "omp.h"
#include "database.h"
#include <QtCore/QCoreApplication>
#include <QtSql>
#include "core.h"

DataBase::DataBase()
{
    dbName = "test";
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

    db = QSqlDatabase::addDatabase("QPSQL",connName);
    db.setDatabaseName(QString::fromStdString(dbName));
    db.setHostName("127.0.0.1");
    db.setUserName("postgres");

    if (!db.open()) {
        fprintf(stderr,"Can not open database %s \n", dbName.c_str());
        //return db.lastError();
    }
    qDebug() << "Connected! And connName is " << connName << " And database name is "
             << QString::fromStdString(dbName) << "\n";
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
    if (!QSqlDatabase::drivers().contains("QPSQL"))
    {
        fprintf(stderr,"Unable to load database\n");
        return false;
    }
    fprintf(stdout,"Driver loaded\n");
    return false;
}

QSqlError DataBase::initDb()
{
    db = QSqlDatabase::addDatabase("QPSQL");
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
    return false;
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
                    ")',NULL,NULL);";
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
                    "',NULL);";

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
            + ",NULL,NULL,"
            + temp->getType()
            + ");";
    query.exec(insert);
    return tid.toStdString();
}

string DataBase::insertData(Sequence sequence, string tbName)
{
    QString insert;
    QString tableName = QString::fromStdString(tbName);
   // if (!isTableExist(tbName))
   // {
        createTable(tbName);
   // }
    //QSqlQuery query;
    QSqlQuery query(db);
    QString start;
    QString end;

    if (getRecordNum(tableName.toStdString()) == 0)
    {
        QString str = "insert into " + tableName
                + " values('',NULL,NULL,NULL);";
        query.exec(str);
    }
    query.clear();
    QString str = "select max(id) from "+tableName+";";
    query.exec(str);
    if(query.next()) {
        start = query.value(0).toString();
    }
    query.clear();
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
                    ")',NULL,NULL);";
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
                    "',NULL);";

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
            + ",NULL,NULL,"
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
    QString str = "create table "+QString::fromStdString(tableName)+"(tid text,pt text,time text, type int,id serial primary key)";
    query.exec(str);
}

vector<Sequence> DataBase::getNSequences(int n, string tableName)
{
    QSqlQuery query(db);
    QString qstr;
    if(n <= 0)
    {
        qstr= "select count(tid) from " +QString::fromStdString(tableName) + " where tid <> ''";
        query.exec(qstr);
        while(query.next())
        {
            n = query.value(0).toInt();
        }
        query.clear();
        qstr= "select max(id) from " +QString::fromStdString(tableName) + " where tid <> ''";
    }
    else
    {
        qstr= "select id from " +QString::fromStdString(tableName) + " where tid <> '' limit " + QString::number(n);
    }
    //loadinto memory

    //select id from Server where tid <> "" limit 10;

    query.exec(qstr);
    QString maxid;
    while(query.next())
    {
        maxid = query.value(0).toString();
    }
    query.clear();

    QString minid;
    if(n <= 0)
    {
        qstr = "select min(id) from " +QString::fromStdString(tableName) + " where tid <> ''";
    }
    else
    {
        qstr= "select id from " +QString::fromStdString(tableName) + " where tid <> '' limit " + QString::number(n);
    }
    query.exec(qstr);
    if(query.first())
    {
        minid = query.value(0).toString();
    }
    else
    {
        minid = maxid;
    }

    qstr = "select * from Server  where id <= " + maxid + "and id > " + minid;
    //tid,pt,time,id,type
    query.exec(qstr);
    QString tid;
    QString pt;
    QString time;
    QString id ;


    vector<Sequence>  res;
    Sequence t[n];
    int i = 0;
    while (query.next()) {
        Point temp;
        tid = query.value(0).toString();
        if(tid.isEmpty())
        {
            pt = query.value(1).toString();
            time = query.value(2).toString();
            id = query.value(4).toString();
            temp.buildPoint(pt,time,id);
            t[i].appendPt(&temp);
        }
        else
        {
            t[i].setID(tid);
            res.push_back(t[i]);
            i++;
        }
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

void DataBase::getNSequences(QMap<QString, Sequence> &id_seq,int n, string tableName)
{
    QSqlQuery query(db);
    QString qstr;
    if(n <= 0)
    {
        qstr= "select count(tid) from " +QString::fromStdString(tableName) + " where tid <> ''";
        query.exec(qstr);
        while(query.next())
        {
            n = query.value(0).toInt();
        }
        query.clear();
        qstr= "select max(id) from " +QString::fromStdString(tableName) + " where tid <> ''";
    }
    else
    {
        qstr= "select id from " +QString::fromStdString(tableName) + " where tid <> '' limit " + QString::number(n);
    }
    //loadinto memory

    //select id from Server where tid <> "" limit 10;
    query.exec(qstr);
    QString maxid;
    while(query.next())
    {
        maxid = query.value(0).toString();
    }

    query.clear();
    qstr = "select * from Server  where id <= " + maxid;
    //tid,pt,time,id,type
    query.exec(qstr);
    QString tid;
    QString pt;
    QString time;
    QString id ;


    Sequence t[n];
    int i = 0;
    while (query.next()) {
        Point temp;
        tid = query.value(0).toString();
        if(tid.isEmpty())
        {
            pt = query.value(1).toString();
            time = query.value(2).toString();
            id = query.value(4).toString();
            temp.buildPoint(pt,time,id);
            t[i].appendPt(&temp);
        }
        else
        {
            t[i].setID(tid);
            id_seq.insert(tid,t[i]);
            i++;
        }
        // qDebug() << pt + time + id;
    }
}

vector<Sequence> DataBase::getNSequences(int n, string tableName, QString type)
{
    QSqlQuery query(db);
    QString qstr;
    if(n <= 0)
    {
        qstr= "select count(tid) from " +QString::fromStdString(tableName) + " where tid <> ''";
        query.exec(qstr);
        while(query.next())
        {
            n = query.value(0).toInt();
        }
        query.clear();
        qstr= "select max(id) from " +QString::fromStdString(tableName) + " where tid <> ''";
    }
    else
    {
        qstr= "select id from " +QString::fromStdString(tableName) + " where tid <> '' limit " + QString::number(n);
    }
    //loadinto memory

    //select id from Server where tid <> "" limit 10;

    query.exec(qstr);
    QString maxid;
    while(query.next())
    {
        maxid = query.value(0).toString();
    }

    query.clear();

    QString minid;
    if(n <= 0)
    {
        qstr = "select min(id) from " +QString::fromStdString(tableName) + " where tid <> ''";
    }
    else
    {
        qstr= "select id from " +QString::fromStdString(tableName) + " where tid <> '' limit " + QString::number(n);
    }
    query.exec(qstr);
    if(query.first())
    {
        minid = query.value(0).toString();
    }
    else
    {
        minid = maxid;
    }


    query.clear();
    qstr = "select * from Server  where id <= " + maxid + "and id > " + minid;
    //tid,pt,time,id,type
    query.exec(qstr);
    QString tid;
    QString pt;
    QString time;
    QString id ;
    QString type_s;

    vector<Sequence>  res;
    Sequence t[n];
    int i = 0;
    while (query.next()) {
        Point temp;
        tid = query.value(0).toString();
        type_s = query.value(3).toString();
        if(tid.isEmpty())
        {
            pt = query.value(1).toString();
            time = query.value(2).toString();
            id = query.value(4).toString();
            temp.buildPoint(pt,time,id);
            t[i].appendPt(&temp);
        }
        else
        {
            t[i].setID(tid);
            t[i].setType(type);
            if(type_s.startsWith(type))
                res.push_back(t[i]);
            i++;
        }
        // qDebug() << pt + time + id;
    }
    return res;
}
QVector<Sequence> DataBase::getAllSequences(string tableName){
    QSqlQuery query(db);
    QString qstr;

    qstr= "select count(tid) from " +QString::fromStdString(tableName) + " where tid <> ''";
    query.exec(qstr);
    query.next();
    int n= query.value(0).toInt();
    query.clear();

    qstr= "select max(id) from " +QString::fromStdString(tableName) + " where tid <> ''";
    query.exec(qstr);
    query.next();
    QString maxid = query.value(0).toString();
    query.clear();
    qstr = "select * from "+QString::fromStdString(tableName)+" where id <= " + maxid;
    query.exec(qstr);

    QString tid;
    QString pt;
    QString time;

    QString id ;
    QVector<Sequence>  res;

    Sequence t[n];
    int i = 0;
    while (query.next()) {
        Point temp;
        tid = query.value(0).toString();
        if(tid.isEmpty())
        {
            pt = query.value(1).toString();
            time = query.value(2).toString();
            id = query.value(4).toString();
            temp.buildPoint(pt,time,id);
            t[i].appendPt(&temp);
        }
        else
        {
            t[i].setID(tid);
            res.append(t[i]);
            i++;
        }
    }
    return res;
}

void DataBase::createResTable()
{
    QSqlQuery query(db);
    /*
     *  _________________________
     * |tid|pyNum|time|id(PK)|res|
     *
     *
     */
    query.exec("create table res(tid text,ptNum int,time int, res double precision, demo text,id serial primary key)");
}

void DataBase::insertIntoResTable(int n, vector<Sequence> sequences, double *res, bool clear)
{
    if(clear)
        clearRes();
    if(static_cast<size_t>(n) != sequences.size())
        return;
    db.transaction();
    QSqlQuery query(db);
    for(int i = 0; i < n; i++) {
        int time = 0;
        if(sequences[i].hasTime())
            time = 1;

        QString str = "insert into res values('";
        str += sequences[i].getID() + "',"
                + QString::number(sequences[i].getNum())
                + ","
                + QString::number(time)
                + ","
                + QString::number(res[i])
                + ",'')";
        query.exec(str);
    }
    db.commit();
}

vector<Result> DataBase::getresult()
{
    vector<Result> resSet;
    QSqlQuery query(db);
    QString str = "select * from res order by res limit 100";
    query.exec(str);

    while (query.next()) {
        Result temp;
        temp.id = query.value(0).toString();
        temp.ptNum = query.value(1).toInt();
        temp.time = query.value(2).toInt();
        temp.res = query.value(3).toDouble();
        temp.demo = query.value(4).toString();
        resSet.push_back(temp);
    }
    return resSet;
}

void DataBase::clearRes()
{
    QSqlQuery query(db);
    QString str = "delete from res";
    query.exec(str);
}

int DataBase::getResNum()
{
    QSqlQuery query(db);
    QString str = "select count(*) from res";
    query.exec(str);
    int count  =  query.value(0).toInt();
    return count;
}


