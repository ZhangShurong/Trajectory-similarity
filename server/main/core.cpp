#include "core.h"
//#include "omp.h"
#include <QDebug>
#include <QtAlgorithms>
#include <time.h>
#include <calwindow.h>
#include "ThirdParty/Eigen/Eigen"

using namespace std;



Core::Core()
{

    tracsLen=0;
    coef = 0;
}
Core::~Core()
{
    if(mem != NULL)
    {
        for (int i =0; i < mX; i++)
        {
            delete [] mem[i];
            mem[i] = NULL;
        }
        delete []mem;
        mem = NULL;
    }
}
void Core::initP_Q(Sequence *m, Sequence *n)
{
    p = m;
    q = n;
}
void Core::initMemSpace(Sequence *p, Sequence *q)
{
    if(mem != NULL)
    {
        for (int i =0; i < mX; i++)
        {
            if(mem[i] != NULL)
                delete [] mem[i];
            mem[i] = NULL;
        }
        delete []mem;
        mem = NULL;
    }
    mX = p->getNum();
    mY = q->getNum();
    mem = new double*[mX];
    for (int k = 0; k < mX; k++)
    {
        mem[k] = new double[mY];
    }
    for (int i = 0; i < mX; i++) {
        for (int j = 0; j < mY; j++) {
            mem[i][j] = -1.0;
        }
    }
}
double Core::computeDiscreteFrechet(Sequence *sa, Sequence *sb)
{
    p = sa;
    q = sb;
    if(sa->hasTime()){
        coef=calCoef();
    }
    initMemSpace(sa, sb);
    return computeDFD(sa->getNum()-1,sb->getNum()-1,p,q);
}

double Core::calCoef() {
    double x1=p->getMaxX()-p->getMinX();
    double y1=p->getMaxY()-p->getMinY();
    double sqr1=sqrt(x1*x1+y1*y1);

    double x2=q->getMaxX()-q->getMinX();
    double y2=q->getMaxY()-q->getMinY();
    double sqr2=sqrt(x2*x2+y2*y2);

    double avgSqr=0.5*(sqr1+sqr2);

    double timeGap1=calTimeDistance(&p->pts[p->getNum()-1],&p->pts[0]);
    double timeGap2=calTimeDistance(&q->pts[q->getNum()-1],&q->pts[0]);

    double timeGapAvg=0.5*(timeGap1+timeGap2);

    if(timeGapAvg==0){
        timeGapAvg=1;
    }

    double coef=avgSqr/timeGapAvg;
    return coef;
}
double Core::computeDFD(int i, int j, Sequence *p_a, Sequence *q_a)
{
    if (mem[i][j] > -1)
        return mem[i][j];
    // if top left column, just compute the distance
    else if (i == 0 && j == 0)
        mem[i][j] = euclideanDistance(&p_a->pts[i], &q_a->pts[j]);
    // can either be the actual distance or distance pulled from above
    else if (i > 0 && j == 0)
        mem[i][j] = max(computeDFD(i - 1, 0,p_a,q_a), euclideanDistance(&p_a->pts[i], &q_a->pts[j]));
    // can either be the distance pulled from the left or the actual
    // distance
    else if (i == 0 && j > 0)
        mem[i][j] = max(computeDFD(0, j - 1,p_a,q_a), euclideanDistance(&p_a->pts[i], &q_a->pts[j]));
    // can be the actual distance, or distance from above or from the left
    else if (i > 0 && j > 0) {
        double temp = min(min(computeDFD(i - 1, j,p_a,q_a), computeDFD(i - 1, j - 1,p_a,q_a)), computeDFD(i, j - 1,p_a,q_a));
        mem[i][j] = max(temp, euclideanDistance(&p_a->pts[i], &q_a->pts[j]));
    }
    // infinite
    else
        mem[i][j] = 10000;

    // printMemory();
    // return the DFD
    return mem[i][j];
}


//分别输入两个轨迹段的起点和终位置
double  Core::getSecSim(int i1,int j1,int i2,int j2) {
    initMemSpace(p, q);
    return computeDFD_new(i1, j1, i2, j2);
}

QVector<SecCompare> Core::findSimilarSection(Sequence *se_a, Sequence *se_b)
{
    QVector<SecCompare> q1;
    QVector<SecCompare> q2;
    p = se_a;
    q = se_b;
    //    initMemSpace(p, q);
    int totalNum=p->pointsNum+q->pointsNum;

    int gap = 1;
    int h = 1;
    double limit=computeDiscreteFrechet(p,q);
    //    int tracsLen=SearchWin::tracLeng;
    if(totalNum<=200&&totalNum>=0){
        gap=1;
        h=1;
    } else {
        gap=5;
        h=gap;
    }

    //#pragma omp parallel for
    for(int i=gap; i<gap+6; i++) {
        calculateSec(i,h,q1);
    }

    //#pragma omp parallel for
    for(int j=gap; j<gap+5; j++) {
        mergeChange(j,q1,q2,limit);
    }
    //  sort(q2.begin(),q2.end(),compare);

    //  #pragma omp parallel for
    //  clock_t t1 = clock();
    for(int i=0; i<q2.size(); i++)
    {
        q1.append(q2[i]);
    }
    //    clock_t t2 = clock();
    //    std::cout<<"time: "<<t2-t1<<std::endl;
    sort(q1.begin(),q1.end(),compare);

    q2.clear();
    return q1;
}

QVector<QVector<int> > Core::getSimplify(Sequence*p,Sequence*q) {
    p->initPainted();
    q->initPainted();
    QVector<QVector<int> >qb;
    QVector<QString>qst;

    // int beginMin1,beginMin2;
    QVector<SecCompare> qs1=findBest(p,q);

    if (qs1.length() != 0)
    {
        //   std::cout<<"time: "<<qs1.length()<<std::endl;
        for (int i = 0; i < qs1.length(); i++)
        {
            QString s =   QString::number(qs1[i].beginIndex1)+" "
                    + QString::number(qs1[i].endIndex1)+" "
                    + QString::number(qs1[i].beginIndex2)+" "
                    + QString::number(qs1[i].endIndex2);
            if(!qst.contains(s))
                qst.append(s);
        }
    }

    for(int k=0; k<qst.length(); k++) {
        QStringList ql=qst[k].split(" ");
        int begin1=ql[0].toInt();
        int end1=ql[1].toInt();
        int begin2=ql[2].toInt();
        int end2=ql[3].toInt();

        for(int j=begin1; j<=end1; j++) {
            p->pts[j].painted=true;
        }
        for(int m=begin2; m<=end2; m++) {
            q->pts[m].painted=true;
        }
    }
    QVector<int>pv;
    QVector<int>qv;
    int k1=0;
    int k2=0;
    for(int i=0; i<p->getNum(); i++) {
        if((i==0&&p->pts[i].painted)||((!p->pts[i-1].painted)&&p->pts[i].painted)||((p->pts[i].painted)&&((i+1<p->getNum())&&(!p->pts[i+1].painted)))||(i+1==p->getNum()&&p->pts[i].painted)) {
            pv.append(i);
            k1++;
        }
    }

    for(int i=0; i<q->getNum(); i++) {
        if((i==0&&q->pts[i].painted)||((!q->pts[i-1].painted)&&q->pts[i].painted)||((q->pts[i].painted)&&((i+1<q->getNum())&&(!q->pts[i+1].painted)))||(i+1==q->getNum()&&q->pts[i].painted)) {
            qv.append(i);
            k2++;
        }
    }
    if(k1!=k2) {
        if(k1>k2) {
            for(int i=0; i<k1-k2; i=i+2) {
                qv.append(qv[k2-2]);
                qv.append(qv[k2-1]);
            }
        } else {
            for(int i=0; i<k2-k1; i=i+2) {
                pv.append(pv[k1-2]);
                pv.append(pv[k1-1]);
            }
        }
    }

    qb.append(pv);
    qb.append(qv);
    qst.clear();
    pv.clear();
    qv.clear();
    return qb;
}

QVector<SecCompare> Core::findBest(Sequence*p,Sequence*q) {
    QVector<SecCompare> temp = getBestSce(findSimilarSection(p, q));
    return temp;
}

bool compare(SecCompare s1,SecCompare s2) {
    return s1.simliarity<s2.simliarity;
}


void Core::calculateSec(int gap, int h, QVector<SecCompare> &q1)
{
    SecCompare s;
    double result;
    // int t=0;
    //     clock_t t1 = clock();

    for(int i=0; i<p->pointsNum-gap; i=i+h) {
        for(int j=0; j<q->pointsNum-gap;  j=j+h) {
            s.beginIndex1=i;
            s.endIndex1=i+gap;
            s.beginIndex2=j;
            s.endIndex2=j+gap;
            result=getSecSim(s.beginIndex1,s.endIndex1,s.beginIndex2,s.endIndex2);
            s.simliarity=result;
            q1.append(s);
        }

    }
}

void Core::mergeChange(int gap, QVector<SecCompare> &q1, QVector<SecCompare> &q2,double limit)
{
    limit=limit*0.1;
    SecCompare s;
    double result;
    //     clock_t t1 = clock();
    // #pragma omp parallel for
    for(int i=0; i<q1.size(); i++) {
        if(q1[i].simliarity<=limit) {
            //            #pragma omp parallel for
            for(int j=0; j<q1.size(); j++) {
                if(q1[j].simliarity<=limit&&q1[j].beginIndex1==q1[i].beginIndex1+gap&&q1[j].beginIndex2==q1[i].beginIndex2+gap) {

                    s.beginIndex1=q1[i].beginIndex1;
                    s.endIndex1=q1[j].endIndex1;
                    s.beginIndex2=q1[i].beginIndex2;
                    s.endIndex2=q1[j].endIndex2;
                    result=getSecSim(s.beginIndex1,s.endIndex1,s.beginIndex2,s.endIndex2);
                    s.simliarity=result;
                    q2.append(s);
                }
            }
        }
    }
}

Sequence* Core::longestCommonSeq(Sequence &p, Sequence &q, double thres)
{
    //Sequence commonSeq[2];
    Sequence *commonSeq = new Sequence[2];
    /* m is a matrix. m[0~Plen][0~Qlen] */
    vector< vector< pair<size_t, char> > > m(p.pointsNum + 1);
    for (size_t i = 0; i < m.size(); ++i) {
        m[i].resize(q.pointsNum + 1);
    }

    for (int i = 1; i <= p.pointsNum; ++i) {
        for (int j = 1; j <= p.pointsNum; ++j) {
            if (euclideanDistance(&p[i - 1], &q[j - 1]) <= thres) {
                m[i][j].first = m[i - 1][j - 1].first + 1;
                m[i][j].second = '\\';
            } else if (m[i][j - 1].first > m[i - 1][j].first) {
                m[i][j].first = m[i][j - 1].first;
                m[i][j].second = '<';
            } else {
                m[i][j].first = m[i - 1][j].first;
                m[i][j].second = '^';
            }
        }
    }

    int i = p.pointsNum, j = q.pointsNum;
    while (m[i][j].first > 0) {
        switch (m[i][j].second) {
        case '<':
            --j;
            break;
        case '^':
            --i;
            break;
        case '\\':
            commonSeq[0].appendPt(&p[i - 1]);
            commonSeq[1].appendPt(&q[j - 1]);
            --i;
            --j;
            break;
        default:
            cerr << "Unrecognized character" << endl;
            break;
        }
    }
    return commonSeq;
}

QVector<SecCompare> Core::getBestSce(QVector<SecCompare> secCompareV_a)
{
    QVector<SecCompare> t;
    // QVector<SecCompare> t1;
    if (secCompareV_a.length() == 0)
    {
        return t;
    }
    int i;
    QVector<QString>qst;
    double similarity  = secCompareV_a[0].simliarity;

    for (i = 0; i < secCompareV_a.length() ; i++)
    {
        if (secCompareV_a[i].simliarity > (similarity+0.0001))
            break;
        QString s = QString::number(secCompareV_a[i].beginIndex1, 10)+" "+ QString::number(secCompareV_a[i].endIndex1, 10)+" "+QString::number(secCompareV_a[i].beginIndex2, 10)+" "+QString::number(secCompareV_a[i].endIndex2, 10);
        if(!qst.contains(s)) {
            qst.append(s);
            t.append(secCompareV_a[i]);
        }
    }
    return t;
}




bool compareDis(PointCompare p1,PointCompare p2) {
    return p1.distance<p2.distance;
}

double Core::computeDFD_new(int startx, int endx, int starty, int endy)
{
    if (mem[endx][endy] > -1)
        return mem[endx][endy];
    // if top left column, just compute the distance
    else if (endx == startx && endy == starty)
        mem[endx][endy] = euclideanDistance(&p->pts[endx],&q->pts[endy]);
    // can either be the actual distance or distance pulled from above
    else if (endx > startx && endy == starty)
        mem[endx][endy] = max(computeDFD_new(startx, endx - 1, starty, endy), euclideanDistance(&p->pts[endx], &q->pts[endy]));
    // can either be the distance pulled from the left or the actual
    // distance
    else if (endx == startx && endy > starty)
        mem[endx][endy] = max(computeDFD_new(startx, endx, starty ,endy - 1), euclideanDistance(&p->pts[endx], &q->pts[endy]));
    // can be the actual distance, or distance from above or from the left
    else if (endx > startx && endy > starty) {
        double temp = min(min(computeDFD_new(startx, endx - 1, starty ,endy), computeDFD_new(startx, endx - 1,starty,  endy - 1)), computeDFD_new(startx, endx, starty, endy - 1));
        mem[endx][endy] = max(temp, euclideanDistance(&p->pts[endx], &q->pts[endy]));
    }
    // infinite
    else
        mem[endx][endy] = 10000;
    // printMemory();
    // return the DFD
    return mem[endx][endy];
}



Point getCenterPoint(Sequence *se_a, int num)
{
    Point pt;
    if (num == 0)
    {
        pt.id = "0";
        pt.longitude = 116.3;
        pt.latitude = 39.9;
        pt.time = "";
        return pt;
    }
    pt.longitude = 0;
    pt.latitude = 0;
    for (int i = 0; i<num; i++)
    {
        pt.longitude += se_a[i].getCentralPoint().longitude;
        pt.latitude += se_a[i].getCentralPoint().latitude;
    }
    pt.longitude = pt.longitude/num;
    pt.latitude = pt.latitude/num;
    return pt;
}

Point getCenterPoint(QVector<Sequence> seqV)
{
    Point pt;
    if (seqV.length() == 0)
    {
        pt.id = "0";
        pt.longitude = 116.3;
        pt.latitude = 39.9;
        pt.time = "";
        return pt;
    }
    pt.longitude = 0;
    pt.latitude = 0;
    for (int i = 0; i<seqV.length(); i++)
    {
        pt.longitude += seqV[i].getCentralPoint().longitude;
        pt.latitude += seqV[i].getCentralPoint().latitude;
    }
    pt.longitude = pt.longitude/seqV.length();
    pt.latitude = pt.latitude/seqV.length();
    return pt;
}
int getZoom(QVector<Sequence> seqV)
{
    double maxLon = seqV[0].getMaxX();
    double maxLat = seqV[0].getMaxY();
    double minLon = seqV[0].getMinX();
    double minLat = seqV[0].getMinY();
    for (int i = 1; i<seqV.length(); i++)
    {
        if (seqV[i].getMaxX() > maxLon)
        {
            maxLon = seqV[i].getMaxX() ;
        }
        if (seqV[i].getMaxY() > maxLat)
        {
            maxLat = seqV[i].getMaxY() ;
        }
        if (seqV[i].getMinX() < minLon)
        {
            minLon = seqV[i].getMinX() ;
        }
        if (seqV[i].getMinY() < minLat)
        {
            minLat = seqV[i].getMinY() ;
        }
    }
    double res = getDistance(minLon, minLat, maxLon, maxLat);
    return calZoomCoef(res);
}

int getZoom(Sequence seq_a)
{
    double maxLon = seq_a.getMaxX();
    double maxLat = seq_a.getMaxY();
    double minLon = seq_a.getMinX();
    double minLat = seq_a.getMinY();
    double res = getDistance(minLon, minLat, maxLon, maxLat);
    return calZoomCoef(res);

}
int calZoomCoef(double res){
    int dis = res / 8.0;
    if ( dis > 100)
    {
        return 6;
    }
    else if (dis > 25)
    {
        return 8;
    }
    else if(dis > 5 )
    {
        return 11;
    }
    else if (dis > 1)
    {
        return 13;
    }
    else if (dis > 0.2)
    {
        return 15;
    }
    else if (dis > 0.05)
    {
        return 16;
    }
    else if(dis > 0.005)
    {
        return 18;
    }

    return 6;
}
double Core::modHausDist(Sequence *sa, Sequence *sb)
{
    if(sa == sb)
    {
        return 0;
    }
    double t1pointsRelPos[sa->pointsNum];
    double t2pointsRelPos[sb->pointsNum];
    int i;
    double distance[sa->pointsNum];
    for(int n = 0; n < sa->pointsNum ; n++)
    {
        distance[n] = 0;
    }
    for(i = 0; i < sa->pointsNum; i++)
    {
        t1pointsRelPos[i] = sa->getPrefixSum(i)/sa->getPrefixSum(sa->pointsNum - 1);
    }
    for(i = 0; i < sb->pointsNum; i++)
    {
        t2pointsRelPos[i] = sb->getPrefixSum(i)/sb->getPrefixSum(sb->pointsNum - 1);
    }
    for(i = 0; i < sa->pointsNum; i++) {
        Point pt1 = sa->pts[i];
        int pt2idx = 0;
        size_t j;
        double temp = 2;//大于1就行
        for(j = 0; j < (size_t)sb->pointsNum; j++) {
            double t = (t1pointsRelPos[i] - t2pointsRelPos[j]);
            if(t < 0)
                t = -t;
            if(t < temp) {
                temp = t;
                pt2idx = j;
            }
        }

        //Get set of points sp2 of sb within neighborhood of point pt2
        vector<size_t> neighborhoodIdxs;
        for(j = 0; j < (size_t)sb->pointsNum; ++j) {
            //FIXME 常数1需要修改
            if((fabs(sb->getPrefixSum(j) - sb->getPrefixSum(pt2idx)) - 1) <= 0)
                neighborhoodIdxs.push_back(j);
        }
        double dist = 9999;//inf
        for(j = 0; j < neighborhoodIdxs.size(); ++j) {
            double newdist = euDistance(pt1, (sb->pts[neighborhoodIdxs.at(j)]));
            if(newdist < dist)
                dist = newdist;
        }
        distance[i] = dist;
    }
    std::sort(distance, distance+sa->pointsNum);
    //FIXME 0.88常数
    return distance[std::min(int(sa->pointsNum * 0.88), sa->pointsNum - 1)];
}
void Core::clusterAgglomerartive(Sequence *seqs, int len)
{
    //createDistanceMatrix
    double distMat[len][len];
    for(int i = 0; i < len; i++)
    {
        for(int j = 0; j < len; j++)
        {
            distMat[i][j] = 1;
            double dist = modHausDist(&seqs[i], &seqs[j]);
            distMat[i][j] = dist;
        }
    }

    vector<vector<int> > clusters;
    for(int i = 0;i < len; i++)
    {
        vector<int> temp;
        temp.push_back(i);
        clusters.push_back(temp);
    }
    int size = len;
    int cn=3;
    while(size > cn)
    {
        double affMat[size][size];
        //TODO 可以用mp加速
        for(int x = 0; x < size; x++)
        {
            for(int y = 0; y < size; y++)
            {
                affMat[x][y] = 0;
            }
        }
        for(int r = 0; r < size - 1; r++)
        {
            for(int c = r+1; c < size; c++)
            {
                double dist = 0;
                vector<int> temp1 = clusters.at(r);
                vector<int> temp2 = clusters.at(c);
                for(uint i = 0; i < temp1.size();i++)
                {
                    int t1idx = temp1.at(i);
                    for(uint j = 0; j < temp2.size();j++)
                    {
                        int t2idx = temp2.at(j);
                        dist += 1 / ((distMat[t1idx][t2idx] * distMat[t2idx][t1idx]) + 0.000006);
                    }
                }
                dist *= 1.0 / (temp1.size() * temp2.size());
                affMat[r][c] = dist;
            }
        }
        double init = -1;
        int t1idx = -1;
        int t2idx = -1;
        for(int i = 0; i < size; i ++)
        {
            for(int j = 0; j < size; j++)
            {
                if(affMat[i][j] > init)
                {
                    init = affMat[i][j];
                    t1idx = i;
                    t2idx = j;
                }
            }
        }
        for (uint i = 0; i < clusters.at(t2idx).size(); i++)
        {
            if(t1idx == t2idx)
                continue;
            clusters.at(t1idx).push_back(clusters.at(t2idx).at(i));
        }
        vector<vector<int> >temp;
        for (uint i = 0; i < clusters.size(); i++)
        {
            if((ssize_t)i != t2idx)
                temp.push_back(clusters.at(i));
        }
        clusters = temp;
        size = clusters.size();

    }
    for(uint i = 0; i < clusters.size();i++)
    {
        qDebug() << "Type "<< i <<":";
        for(size_t j = 0; j< clusters.at(i).size(); j++)
        {
            qDebug() << clusters.at(i).at(j);
        }
    }
}
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
    if(--depth < 0)
        return 0;
    if(q->getMaxX() > midLongtitude) {
        if(q->getMinX() < midLatitude)
            return 0;
        if(q->getMaxY() > midLatitude) {
            if(q->getMinY() < midLatitude)
                return 0;
            else {

                //                double min2Longtitude = midLongtitude;
                //                double max2Longtitude = maxLongtitude;
                //                double min2Latitude = midLatitude;
                //                double max2Latitude = maxLatitude;
                int res = hardCluster(q, midLongtitude,
                                      maxLongtitude,
                                      midLongtitude,
                                      maxLatitude,
                                      depth);
                if(res != 0) {
                    int n = 1 + (int)log10(res);
                    return 2*pow(10,n) +res ;
                }
                else
                    return 2;
            }
        } else {
            //    double min4Longtitude = midLongtitude;
            //    double max4Longtitude = maxLongtitude;
            //    double min4Latitude = minLatitude;
            //    double max4Latitude = midLatitude;
            int res = hardCluster(q,midLongtitude,
                                  maxLongtitude,
                                  minLatitude,
                                  midLatitude,
                                  depth);
            if(res != 0) {
                int n = 1 + (int)log10(res);
                return 4*pow(10,n) +res ;
            } else {
                return 4;
            }
        }
    } else {
        if(q->getMaxY() > midLatitude) {
            if(q->getMinY() < midLatitude) {
                return 0;
            } else {
                //    double min1Longtitude = minLongtitude;
                //    double max1Longtitude = midLongtitude;
                //    double min1Latitude = midLatitude;
                //    double max1Latitude = maxLatitude;
                int res =hardCluster(q,minLongtitude,
                                     midLongtitude,
                                     midLatitude,
                                     maxLatitude,
                                     depth);
                if(res != 0) {
                    int n = 1 + (int)log10(res);
                    return 1*pow(10,n) +res ;
                } else
                    return 1;
            }
        } else {
            //    double min3Longtitude = minLongtitude;
            //    double max3Longtitude = midLongtitude;
            //    double min3Latitude = minLatitude;
            //    double max3Latitude = midLatitude;
            int res = hardCluster(q, minLongtitude,
                                  midLongtitude,
                                  minLatitude,
                                  midLatitude,
                                  depth);
            if(res != 0) {
                int n = 1 + (int)log10(res);
                return 3*pow(10,n) +res ;
            } else
                return 3;
        }
    }
    return 0;
}
bool Core::compareType(QString input_type, QString type)
{
    return input_type == type || input_type.startsWith(type) ||
            input_type == "0" || type == "0";
}
void Core::normalize(Sequence &se)
{
    if(se.getMaxX() == se.getMinX()) {
        double midLong = 0.5 * (MAXLONG - MINLONG);
        for(int i = 0; i < se.pointsNum; ++i)
            se.pts[i].longitude = midLong;
    } else {
        double k = (MAXLONG - MINLONG)/(se.getMaxX() - se.getMinX());
        double b = MINLONG - k*se.getMinX();
        for(int i = 0; i < se.pointsNum; i++)
            se.pts[i].longitude = k*se.pts[i].longitude + b;
    }
    if(se.getMaxY() == se.getMinY()) {
        double midLa = 0.5 * (MAXLA - MINLA);
        for(int i = 0; i < se.pointsNum; i++)
            se.pts[i].latitude = midLa;
    } else {
        double k = (MAXLA - MINLA)/(se.getMaxY() - se.getMinY());
        double b = MINLA - k*se.getMinY();
        for(int i = 0; i < se.pointsNum; i++)
            se.pts[i].latitude = k*se.pts[i].latitude + b;
    }
}
void Core::clusterSpectral()
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
vector<int> Core::clusterAgglomerartive(vector<Sequence> seqs)
{
    int len = seqs.size();
    double distMat[len][len];
    for(int i = 0; i < len; i++)
    {
        for(int j = 0; j < len; j++)
        {
            distMat[i][j] = 1;
            double dist = modHausDist(&seqs[i], &seqs[j]);
            distMat[i][j] = dist;
        }
    }

    vector<vector<int> > clusters;
    for(int i = 0;i < len; i++)
    {
        vector<int> temp;
        temp.push_back(i);
        clusters.push_back(temp);
    }
    int size = len;
    int cn=3;
    while(size > cn)
    {
        double affMat[size][size];
        //TODO 可以用mp加速
        for(int x = 0; x < size; x++)
        {
            for(int y = 0; y < size; y++)
            {
                affMat[x][y] = 0;
            }
        }
        for(int r = 0; r < size - 1; r++)
        {
            for(int c = r+1; c < size; c++)
            {
                double dist = 0;
                vector<int> temp1 = clusters.at(r);
                vector<int> temp2 = clusters.at(c);
                for(uint i = 0; i < temp1.size();i++)
                {
                    int t1idx = temp1.at(i);
                    for(uint j = 0; j < temp2.size();j++)
                    {
                        int t2idx = temp2.at(j);
                        dist += 1 / ((distMat[t1idx][t2idx] * distMat[t2idx][t1idx]) + 0.000006);
                    }
                }
                dist *= 1.0 / (temp1.size() * temp2.size());
                affMat[r][c] = dist;
            }
        }
        double init = -1;
        ssize_t t1idx = -1;
        ssize_t t2idx = -1;
        for(int i = 0; i < size; i ++)
        {
            for(int j = 0; j < size; j++)
            {
                if(affMat[i][j] > init)
                {
                    init = affMat[i][j];
                    t1idx = i;
                    t2idx = j;
                }
            }
        }
        for (uint i = 0; i < clusters.at(t2idx).size(); i++)
        {
            if(t1idx == t2idx)
                continue;
            clusters.at(t1idx).push_back(clusters.at(t2idx).at(i));
        }
        vector<vector<int> >temp;
        for (uint i = 0; i < clusters.size(); i++)
        {
            if((ssize_t)i != t2idx)
                temp.push_back(clusters.at(i));
        }
        clusters = temp;
        size = clusters.size();
    }
    vector<int> res;
    for(uint i = 0; i < seqs.size();i++)
    {
        res.push_back(-1);
    }
    for(uint i = 0; i < clusters.size();i++)
    {
        for(uint j = 0; j< clusters.at(i).size(); j++)
        {
            res.at(clusters.at(i).at(j)) = i;
        }
    }
    return res;
}
void getSquFromFile(Csv *csv, Sequence *se)
{
    if (se->getNum() != 0)
    {
        delete se;
        Sequence *t = new Sequence();
        se = t;
    }
    se->setID("Input");
    string line;
    bool ok;
    QString xStr;
    QString yStr;
    double x;
    double y;

    QVector<Point*> tContainer;
    while (csv->getline(line) != 0) {
        const char *t = line.c_str();
        if (Csv::notStdAscii(t[0]))
            continue;
        xStr = QString::fromStdString(csv->getfield(0));
        yStr = QString::fromStdString(csv->getfield(1));
        x = xStr.toDouble(&ok);
        if (ok)
        {
            y = yStr.toDouble(&ok);
            if(ok)
            {
                int j =csv->getnfield();
                if ( j== 2)
                {
                    Point *temp = new Point(x,y);
                    se->appendPt(temp);
                }
                else if (j == 3) {
                    Point *temp = new Point(x,y,QString::fromStdString(csv->getfield(2)));
                    temp->t=loadToStruct(temp->time);
                    tContainer.append(temp);
                }
                else {
                    fprintf(stderr, "Wrong\n");
                    return;
                }
            }
        }
    }
    if(!tContainer.isEmpty()) {
        sort(tContainer.begin(),tContainer.end(),timeCompare);
        for(int i=0; i<tContainer.size(); i++) {
            se->appendPt(tContainer[i]);
        }
    }
    double minLongtitude = 73;
    double maxLongtitude = 136;
    double minLatitude = 3;
    double maxLatitude = 54;
    int type = hardCluster(se,
                           minLongtitude,
                           maxLongtitude,
                           minLatitude,
                           maxLatitude,
                           3
                           );
    se->setType(QString::number(type));
    tContainer.clear();
}
Time loadToStruct(QString time) {
    QString re = "\\d{8}[\\s]+[\\d]+:[\\d]+:[\\d]+";
    QRegExp rx(re);
    bool match = rx.exactMatch(time);
    if(!match)
    {
        throw 0;
    }

    Time t;
    QStringList tl=time.split(QRegExp(QRegExp("[\\s]+")));
    t.year=tl[0].mid(0,4).toInt();
    t.month=tl[0].mid(4,2).toInt();
    t.day=tl[0].mid(6,2).toInt();

    QStringList tl1=tl[1].split(":");
    t.hour=tl1[0].toInt();
    t.minute=tl1[1].toInt();
    t.second=tl1[2].toInt();
    return t;
}
double Core::calTimeDistance(Point *a,Point *b) {
    double hourGap;
    if((a->t.year!=b->t.year)||(a->t.month!=b->t.month)) {
        hourGap=720;
    } else {
        hourGap=qAbs((a->t.day*24+a->t.hour+a->t.minute*0.016+a->t.second*0.0002)-(b->t.day*24+b->t.hour+b->t.minute*0.016+b->t.second*0.0002));
    }
    return hourGap;
}
bool timeCompare(Point*p1,Point*p2) {
    Time* t1=&p1->t;
    Time* t2=&p2->t;

    if(t1->year<t2->year) {
        return true;
    } else if(t1->year==t2->year) {
        if(t1->month<t2->month) {
            return true;
        } else if(t1->month==t2->month) {
            if(t1->day<t2->day) {
                return true;
            } else if(t1->day==t2->day) {
                int totalSecond1=t1->hour*3600+t1->minute*60+t1->second;
                int totalSecond2=t2->hour*3600+t2->minute*60+t2->second;
                return totalSecond1<=totalSecond2;
            }else{
                return false;
            }
        }else{
            return false;
        }
    }else {
        return false;
    }
}
QVector<PointCompare> Core::getNearestPoint(Sequence *se_a, Sequence *se_b) {
    QVector<PointCompare>q1;
    QVector<PointCompare>q2;
    PointCompare p;
    for(int i=0; i<se_a->getNum(); i++) {
        for(int j=0; j<se_b->getNum(); j++) {
            p.distance=euclideanDistance(&se_a->pts[i],&se_b->pts[j]);
            p.index1=i;
            p.index2=j;
            q1.append(p);
        }
    }
    sort(q1.begin(),q1.end(),compareDis);
    double sim=q1[0].distance;
    for(int i=0; i<q1.size(); i++) {
        if(q1[i].distance<=sim) {
            q2.append(q1[i]);

        } else {
            break;
        }
    }
    return q2;
}
double rad(double d)
{
    return d * PI / 180.0;
}
double getDistance(double lng1, double lat1, double lng2, double lat2)
{
    double radLat1 = rad(lat1);
    double radLat2 = rad(lat2);
    double a = radLat1 - radLat2;
    double b = rad(lng1) - rad(lng2);
    double s = 2 * asin(sqrt(pow(sin(a/2),2) +
                             cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
    s = s * EARTH_RADIUS;
    s = round(s * 10000) / 10000;
    return s;
}
double Core::euclideanDistance(Point* a, Point* b)
{
    double dis = 0;
    if(a->time.isEmpty()) {
        dis = (a->longitude - b->longitude)*(a->longitude - b->longitude)
                + (a->latitude - b->latitude)*(a->latitude - b->latitude);
    } else {
        dis = (a->longitude - b->longitude)*(a->longitude - b->longitude)
                + (a->latitude - b->latitude)*(a->latitude - b->latitude)+calTimeDistance(a,b)*coef;
    }
    //    dis = sqrt(dis);
    return dis;
}

double euDistance(Point a, Point b)
{
    double dis = 0;
    dis = (a.longitude - b.longitude)*(a.longitude - b.longitude)
            + (a.latitude - b.latitude)*(a.latitude - b.latitude);
    //    dis = sqrt(dis);
    return dis;
}
