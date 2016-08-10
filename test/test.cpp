#include <iostream>
#include "sequence.h"
#include "core.h"
#include "csv.h"
#include <QApplication>
#include <QDebug>
#include <cmath>


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

//double minLongtitude = 73;
//double maxLongtitude = 136;
//double minLatitude = 3;
//double maxLatitude = 54;




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Sequence seqs[6];

//    openfile(&seqs[0],"./data/y=1.5x2.csv");
    openfile(&seqs[5],"./data/test.csv");
    openfile(&seqs[1],"./data/y=x2.csv");
    openfile(&seqs[2],"./data/y=0.csv");
    openfile(&seqs[0],"./data/y=2x2.csv");
    openfile(&seqs[4],"./data/y=sinx.csv");
    openfile(&seqs[3],"./data/y=x.csv");

    //double res =  modHausDist(&seqs[0],&seqs[3]);
    clusterAgglomerartive(seqs,6);
    //qDebug() << (int)res;

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
    return a.exec();
}
