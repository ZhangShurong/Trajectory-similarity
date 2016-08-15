#include <iostream>
#include "sequence.h"
#include "core.h"
#include "csv.h"
#include <QApplication>
#include <QDebug>
#include <cmath>
#include "testqdatastream.h"

void openfile(Sequence *p, string file_name)
{
    string fileName = file_name;
    ifstream fin(fileName.c_str());
    if(!fin.is_open())
    {
        return;
    }
    Csv csv(fin);
    try
    {
        getSquFromFile(&csv,p);
    }
    catch(int i)
    {
        delete p;
        p = new Sequence();
        fin.close();
        return;
    }
    p->setID("p");
    fin.close();
}

void testCluster()
{
    Sequence seqs[6];

    openfile(&seqs[5],"./data/test.csv");
    openfile(&seqs[1],"./data/y=x2.csv");
    openfile(&seqs[2],"./data/y=0.csv");
    openfile(&seqs[0],"./data/y=2x2.csv");
    openfile(&seqs[4],"./data/y=sinx.csv");
    openfile(&seqs[3],"./data/y=x.csv");

    Core core;
    core.clusterAgglomerartive(seqs,6);


    double minLongtitude = 73;
    double maxLongtitude = 136;
    double minLatitude = 3;
    double maxLatitude = 54;
    qDebug() << "___________" << hardCluster(&seqs[1],
            minLongtitude,
            maxLongtitude,
            minLatitude,
            maxLatitude,
            3
            );
}
void testQdata()
{
    TestQdataStream testQdataStream;
    testQdataStream.a = 1;
    testQdataStream.b = 4;
    testQdataStream.c = 5;
    qDebug() << testQdataStream.getSum();
    QFile f( "file.dat" );
    f.open(QIODevice::WriteOnly );
    QDataStream out(&f);
    out << testQdataStream;
    f.close();

    f.open(QIODevice::ReadOnly);
    TestQdataStream t;
    QDataStream in(&f);
    in >> t;
    qDebug()<<t.getSum();
    f.close();

    Sequence t1;
    qDebug() << t1.getNum();
    openfile(&t1, "./data/test_1.csv");

    QFile sef("test.csv");
    sef.open(QIODevice::WriteOnly);
    QDataStream se(&sef);
    se << t1;
    sef.close();

    Sequence t2;
    sef.open(QIODevice::ReadOnly);
    QDataStream se2(&sef);
    se2 >> t2;
    sef.close();
    qDebug() << t2.getNum();

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    testQdata();
    qDebug() << "Hello World";
    return a.exec();
}
