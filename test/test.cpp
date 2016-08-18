#include <iostream>
#include "sequence.h"
#include "core.h"
#include "csv.h"
#include <QApplication>
#include <QDebug>
#include <cmath>
#include "testqdatastream.h"
#include "ThirdParty/Eigen/Eigen"

void clusterSpectral()
{
    int len = 5;
    //createDistanceMatrix()
    double rowSortedDistMat[len][len];
    double distMat[len][len];
    double K[len][len];
    for(int i = 0; i < len; i++) {
        for(int j = 0; j < len; j++) {
            distMat[i][j] = 1;
            double dist = i+j;
            distMat[i][j] = dist;
            rowSortedDistMat[i][j] = dist;
            K[i][j] = 0;
        }
    }

    //createStdDevs()
    //rowSortedDistMat.sort(axis = 1)
    for(int i = 0; i < len; i++) {
        std::sort(rowSortedDistMat[i],rowSortedDistMat[i]+len);
    }

    //FIXME stdNN = 2????
    //self.stdDevs = rowSortedDistMat[:, min(self.stdNN, rowSortedDistMat.shape[1] - 1)]
    int stdNN = 2;
    double stdDev[len];
    int t = stdNN < len?stdNN:len;
    for(int i = 0;i < len; i++) {
        stdDev[i] = rowSortedDistMat[i][t];
        std::cout << stdDev[i] << ", ";
    }
    /*
     *         for i in range(len(self.stdDevs)):
            self.stdDevs[i] = max(self.stdMin, min(self.stdMax, self.stdDevs[i]))
     */
    double stdMin = 0.4;
    double stdMax = 20.0;
    for(int i = 0; i < len;i++) {
        stdDev[i] = std::max(stdMin, std::min(stdMax,stdDev[i]));
    }
    std::cout << std::endl;

    // Compute affinity matrix
    //std::exp( -(distMat[r][c] * distMat[r][c]) / (2 * stdDev(r) * stdDev(c)));
    for(int r = 0; r < len; r++)
    {
        for(int c = 0; c < len; c++)
        {
            K[r][c] = std::exp( -(distMat[r][c] * distMat[r][c]) / (2 * stdDev[r] * stdDev[c]));
        }
    }
    //W = np.diag(1.0 / np.sqrt(np.sum(K, 1)))
    double W[len][len];
    for(int i = 0;i< len;i++) {
        double sum = 0;
        for(int j = 0; j< len;j++) {
            W[i][j] = 0;
            sum += K[i][j];
        }
        W[i][i] = 1/std::sqrt(sum);
    }


    //# Normalized affinity matrix
    //L = np.dot(np.dot(W, K), W)

    //W*K
    double temp[len][len];
    for(int i = 0; i < len;i++) {

        for(int j = 0; j< len;j++) {
            double sum = 0;
            for(int k = 0; k< len;k++) {
                sum += W[i][k] * K[k][j];
            }
            temp[i][j] = sum;
        }
    }
    //temp.*W
    Eigen::MatrixXd L(len,len);
    //double L[len][len];
    for(int i = 0; i < len;i++)
    {
        for(int j = 0; j< len;j++)
        {
            double sum = 0;
            for(int k = 0; k< len;k++)
            {
                sum += temp[i][k] * W[k][j];
            }
            L(i,j) = sum;
        }
    }
    std::cout <<"L:-----------------------------\n";
    std::cout<< L << std::endl;

    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigenSolver(L);
    Eigen::MatrixXd eigenvalue(len,1);
    Eigen::MatrixXd eigenvectors(len,len);
    if (eigenSolver.info() == Eigen::Success) {
        eigenvalue = eigenSolver.eigenvalues();
        std::cout << "eigenvalues:\n" << eigenvalue << std::endl;
        eigenvectors = eigenSolver.eigenvectors();
        std::cout << "eigenvectors:\n"<< eigenvectors << std::endl;
    }

    double gMin = 0;
    double gMax = 0;
    for(int i = 0; i < len; i++)
    {
        if(eigenvalue(i) > 0.8)
        {
            gMax += 1;
        }
        if(eigenvalue(i) > 0.99)
        {
            gMin += 1;
        }
    }
    //int clusters = -1;
    //int g = clusters;
}
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
    clusterSpectral();
    qDebug() << "Hello World";
    return a.exec();
}
