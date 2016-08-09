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


/*
 * 矩形按照如下方式递归分割
 *  ______________________
 * |          |           |
 * |     1    |     2     |
 * |          |           |
 * |——————————|———————————|
 * |          |           |
 * |     3    |     4     |
 * |__________|___________|
 *
 */
int hardCluster(Sequence * q,double minLongtitude,
                double maxLongtitude,
                double minLatitude,
                double maxLatitude,
                int depth)
{
    double midLongtitude = 0.5*(maxLongtitude + minLongtitude);
    double midLatitude = 0.5*(maxLatitude + minLatitude);
    depth --;
    if(depth < 0)
    {
        return 0;
    }
    if(q->getMaxX() > midLongtitude)
    {
        if(q->getMinX() < midLatitude)
        {
            return 0;
        }
        if(q->getMaxY() > midLatitude)
        {
            if(q->getMinY() < midLatitude)
            {
                return 0;
            }
            else
            {

//                double min2Longtitude = midLongtitude;
//                double max2Longtitude = maxLongtitude;
//                double min2Latitude = midLatitude;
//                double max2Latitude = maxLatitude;
                int res = hardCluster(q, midLongtitude,
                                      maxLongtitude,
                                      midLongtitude,
                                      maxLatitude,
                                      depth);
                if(res != 0)
                {
                    int n = 1 + (int)log10(res);
                    return 2*pow(10,n) +res ;
                }
                else
                    return 2;
            }
        }
        else
        {
            //    double min4Longtitude = midLongtitude;
            //    double max4Longtitude = maxLongtitude;
            //    double min4Latitude = minLatitude;
            //    double max4Latitude = midLatitude;
            int res = hardCluster(q,midLongtitude,
                                  maxLongtitude,
                                  minLatitude,
                                  midLatitude,
                                  depth);
            if(res != 0)
            {
                int n = 1 + (int)log10(res);
                return 4*pow(10,n) +res ;
            }
            else
            {
                return 4;
            }

        }
    }
    else
    {
        if(q->getMaxY() > midLatitude)
        {
            if(q->getMinY() < midLatitude)
            {
                return 0;
            }
            else
            {
                //    double min1Longtitude = minLongtitude;
                //    double max1Longtitude = midLongtitude;
                //    double min1Latitude = midLatitude;
                //    double max1Latitude = maxLatitude;
                int res =hardCluster(q,minLongtitude,
                                     midLongtitude,
                                     midLatitude,
                                     maxLatitude,
                                     depth);
                if(res != 0)
                {
                    int n = 1 + (int)log10(res);
                    return 1*pow(10,n) +res ;
                }
                else
                    return 1;
            }
        }
        else
        {
            //    double min3Longtitude = minLongtitude;
            //    double max3Longtitude = midLongtitude;
            //    double min3Latitude = minLatitude;
            //    double max3Latitude = midLatitude;
            int res = hardCluster(q, minLongtitude,
                                     midLongtitude,
                                     minLatitude,
                                     midLatitude,
                                     depth);
            if(res != 0)
            {
                int n = 1 + (int)log10(res);
                return 3*pow(10,n) +res ;
            }
            else
                return 3;
        }
    }
    return 0;


}

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
