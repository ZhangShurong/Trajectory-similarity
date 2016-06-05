#include "core.h"
#include <QDebug>
#include <QtAlgorithms>
#include <time.h>
#include <calwindow.h>

using namespace std;
double **mem = NULL;
Sequence *p;
Sequence *q;
double coef;
int mX;
int mY;


double computeDiscreteFrechet(Sequence *sa, Sequence *sb)
{
    p = sa;
    q = sb;
    coef=calCoef();
    initMemSpace(sa, sb);
    return computeDFD(sa->getNum()-1,sb->getNum()-1,p,q);
}
double euclideanDistance(Point a, Point b)
{
    double dis = 0;
    if(a.time.isEmpty()){
        dis = (a.longitude - b.longitude)*(a.longitude - b.longitude)
                + (a.latitude - b.latitude)*(a.latitude - b.latitude);
    }else {
        dis = (a.longitude - b.longitude)*(a.longitude - b.longitude)
                + (a.latitude - b.latitude)*(a.latitude - b.latitude)+calTimeDistance(a,b)*coef;
   }
    dis = sqrt(dis);
    return dis;
}

double calTimeDistance(Point &a,Point &b){

//     Time t1=loadToStruct(a.time);
//     Time t2=loadToStruct(b.time);

    double hourGap;

    if((a.t.year!=b.t.year)||(a.t.month!=b.t.month)){
        hourGap=720;
    }else{
        hourGap=qAbs((a.t.day*24+a.t.hour+a.t.minute*0.016+a.t.second*0.0002)-(b.t.day*24+b.t.hour+b.t.minute*0.016+b.t.second*0.0002));
    }
    return hourGap;
}

double calCoef(){
    double x1=p->getMaxX()-p->getMinX();
    double y1=p->getMaxY()-p->getMinY();
    double sqr1=sqrt(x1*x1+y1*y1);

    double x2=q->getMaxX()-q->getMinX();
    double y2=q->getMaxY()-q->getMinY();
    double sqr2=sqrt(x2*x2+y2*y2);

    double avgSqr=0.5*(sqr1+sqr2);

    double timeGap1=calTimeDistance(p->pts[p->getNum()-1],p->pts[0]);
    double timeGap2=calTimeDistance(q->pts[q->getNum()-1],q->pts[0]);

    double timeGapAvg=0.5*(timeGap1+timeGap2);

    double coef=avgSqr/timeGapAvg;
    return coef;
}

void
getSquFromFile(Csv *csv, Sequence *se)
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
        if(!tContainer.isEmpty()){
            sort(tContainer.begin(),tContainer.end(),timeCompare);
            for(int i=0;i<tContainer.size();i++){
                se->appendPt(tContainer[i]);
            }
        }
        tContainer.clear();
}


Time loadToStruct(QString time){
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

bool timeCompare(Point*p1,Point*p2){
    Time t1=p1->t;
    Time t2=p2->t;

    if(t1.year<t2.year){
        return true;
    }else if(t1.year==t2.year){
        if(t1.month<t2.month){
            return true;
        }else if(t1.month==t2.month){

            if(t1.day<t2.day){
                return true;
            }else if(t1.day==t2.day){
                int totalSecond1=t1.hour*3600+t1.minute*60+t1.second;
                int totalSecond2=t2.hour*3600+t2.minute*60+t2.second;
                if(totalSecond1<=totalSecond2){
                    return true;
                }else{
                    return false;
                }
            }else if(t1.day>t2.day){
                 return false;
            }

        }else if(t1.month>t2.month){
             return false;
        }

    }else if(t1.year>t2.year){
         return false;
    }
    return false;
}

void initMemSpace(Sequence *p, Sequence *q)
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

//分别输入两个轨迹段的起点和终位置
double  getSecSim(int i1,int j1,int i2,int j2){
    initMemSpace(p, q);

    return computeDFD_new(i1, j1, i2, j2);
}

QVector<SecCompare> findSimilarSection(Sequence *se_a, Sequence *se_b)
{
      QVector<SecCompare> q1;
      QVector<SecCompare> q2;
      QVector<SecCompare> q3;

      p = se_a;
      q = se_b;
       int totalNum=p->pointsNum+q->pointsNum;

       int gap = 1;
       int h = 1;
       double limit=computeDiscreteFrechet(p,q);

      if(totalNum<=200&&totalNum>=0){
               gap=1;
               h=1;
       }else if(totalNum>200){
           gap=5;
           h=gap;
       }
         for(int i=gap;i<gap+6;i++){
               calculateSec(i,h,q1);
          }

        for(int j=gap;j<gap+5;j++){
               mergeChange(j,q1,q2,limit);
        }
        sort(q2.begin(),q2.end(),compare);
        for(int i=0;i<q2.size();i++)
        {
            q1.append(q2[i]);
//            if(i>100)
//                break;
        }
        sort(q1.begin(),q1.end(),compare);

//        if(q1.size()>1000){
//            for(int i=0;i<1000;i++){
//                q3.append(q1[i]);
//            }
//        }else{
//            for(int i=0;i<q1.size();i++){
//                q3.append(q1[i]);
//            }
//        }
//        q1.clear();
        q2.clear();
        return q1;
}

QVector<QVector<int> > getSimplify(Sequence*p,Sequence*q){
	p->initPainted();
	q->initPainted();
    QVector<QVector<int> >qb;
    QVector<QString>qst;

   // int beginMin1,beginMin2;
    QVector<SecCompare> qs1=findBest(p,q);

    if (qs1.length() != 0)
     {
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

    for(int k=0;k<qst.length();k++){
        QStringList ql=qst[k].split(" ");
        int begin1=ql[0].toInt();
        int end1=ql[1].toInt();
        int begin2=ql[2].toInt();
        int end2=ql[3].toInt();

        for(int j=begin1;j<=end1;j++){
            p->pts[j].painted=true;
        }
        for(int m=begin2;m<=end2;m++){
            q->pts[m].painted=true;
        }
    }
         QVector<int>pv;
         QVector<int>qv;
         int k1=0;int k2=0;
         for(int i=0;i<p->getNum();i++){
             if((i==0&&p->pts[i].painted)||((!p->pts[i-1].painted)&&p->pts[i].painted)||((p->pts[i].painted)&&((i+1<p->getNum())&&(!p->pts[i+1].painted)))||(i+1==p->getNum()&&p->pts[i].painted)){
                           pv.append(i);
                           k1++;
                       }
                   }

         for(int i=0;i<q->getNum();i++){
               if((i==0&&q->pts[i].painted)||((!q->pts[i-1].painted)&&q->pts[i].painted)||((q->pts[i].painted)&&((i+1<q->getNum())&&(!q->pts[i+1].painted)))||(i+1==q->getNum()&&q->pts[i].painted)){
                           qv.append(i);
                           k2++;
                       }
                   }
         if(k1!=k2){
             if(k1>k2){
                 for(int i=0;i<k1-k2;i=i+2){
                     qv.append(qv[k2-2]);
                     qv.append(qv[k2-1]);
                 }
             }else{
                 for(int i=0;i<k2-k1;i=i+2){
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


QVector<SecCompare> findBest(Sequence*p,Sequence*q){
    QVector<SecCompare> temp = getBestSce(findSimilarSection(p, q));
    return temp;
}


bool compare(SecCompare s1,SecCompare s2){
    return s1.simliarity<s2.simliarity;
}


double computeDFD(int i, int j, Sequence *p_a, Sequence *q_a)
{
    if (mem[i][j] > -1)
        return mem[i][j];
    // if top left column, just compute the distance
    else if (i == 0 && j == 0)
        mem[i][j] = euclideanDistance(p_a->pts[i], q_a->pts[j]);
    // can either be the actual distance or distance pulled from above
    else if (i > 0 && j == 0)
        mem[i][j] = max(computeDFD(i - 1, 0,p_a,q_a), euclideanDistance(p_a->pts[i], q_a->pts[j]));
    // can either be the distance pulled from the left or the actual
    // distance
    else if (i == 0 && j > 0)
        mem[i][j] = max(computeDFD(0, j - 1,p_a,q_a), euclideanDistance(p_a->pts[i], q_a->pts[j]));
    // can be the actual distance, or distance from above or from the left
    else if (i > 0 && j > 0) {
        double temp = min(min(computeDFD(i - 1, j,p_a,q_a), computeDFD(i - 1, j - 1,p_a,q_a)), computeDFD(i, j - 1,p_a,q_a));
        mem[i][j] = max(temp, euclideanDistance(p_a->pts[i], q_a->pts[j]));
    }
    // infinite
    else
        mem[i][j] = 10000;

    // printMemory();
    // return the DFD
    return mem[i][j];
}

void calculateSec(int gap, int h, QVector<SecCompare> &q1)
{
   // int t=0;
    for(int i=0;i<p->pointsNum-gap;i=i+h){
        for(int j=0;j<q->pointsNum-gap;  j=j+h){
      SecCompare s;
      s.beginIndex1=i;
      s.endIndex1=i+gap;
      s.beginIndex2=j;
      s.endIndex2=j+gap;
      double result=getSecSim(s.beginIndex1,s.endIndex1,s.beginIndex2,s.endIndex2);
      s.simliarity=result;
      q1.append(s);
      //t++;
    }
//        if(t>1000)
//            break;
    }
}

void mergeChange(int gap, QVector<SecCompare> &q1, QVector<SecCompare> &q2,double limit)
{
 //   double res = computeDiscreteFrechet(p,q);
   // double limit = 0.8;
    limit=limit*0.1;
    for(int i=0;i<q1.size();i++){
        if(q1[i].simliarity<=limit){
            for(int j=0;j<q1.size();j++){
                if(q1[j].simliarity<=limit&&q1[j].beginIndex1==q1[i].beginIndex1+gap&&q1[j].beginIndex2==q1[i].beginIndex2+gap){
                    SecCompare s;
                    s.beginIndex1=q1[i].beginIndex1;
                    s.endIndex1=q1[j].endIndex1;
                    s.beginIndex2=q1[i].beginIndex2;
                    s.endIndex2=q1[j].endIndex2;
                    double result=getSecSim(s.beginIndex1,s.endIndex1,s.beginIndex2,s.endIndex2);
                    s.simliarity=result;
                    q2.append(s);
            }
        }
    }
}
}

Sequence* longestCommonSeq(Sequence &p, Sequence &q, double thres)
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
            if (euclideanDistance(p[i - 1], q[j - 1]) <= thres) {
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

QVector<SecCompare> getBestSce(QVector<SecCompare> secCompareV_a)
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
                    if(!qst.contains(s)){
                         qst.append(s);
                         t.append(secCompareV_a[i]);
                    }
      }
    return t;
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
    for (int i = 0;i<num;i++)
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
    for (int i = 0;i<seqV.length();i++)
    {
        pt.longitude += seqV[i].getCentralPoint().longitude;
        pt.latitude += seqV[i].getCentralPoint().latitude;
    }
    pt.longitude = pt.longitude/seqV.length();
    pt.latitude = pt.latitude/seqV.length();
    return pt;
}

QVector<PointCompare> getNearestPoint(Sequence *se_a, Sequence *se_b){
    QVector<PointCompare>q1;
   // PointCollection pc;
    QVector<PointCompare>q2;

    for(int i=0;i<se_a->getNum();i++){
        for(int j=0;j<se_b->getNum();j++){
            PointCompare p;
            p.distance=euclideanDistance(se_a->pts[i],se_b->pts[j]);
            p.index1=i;
            p.index2=j;
            q1.append(p);
        }
    }
    sort(q1.begin(),q1.end(),compareDis);
    double sim=q1[0].distance;
    for(int i=0;i<q1.size();i++){
        if(q1[i].distance<=sim){
            //  pc.p1.append(q1[i].index1);
              //pc.p2.append(q1[i].index2);
            q2.append(q1[i]);

        }else {
            break;
        }
    }
     // return pc;
    return q2;
}

bool compareDis(PointCompare p1,PointCompare p2){
   return p1.distance<p2.distance;
}


void out2DArray(double **arr, int x, int y)
{
    int i;
    for (i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            printf("%4f\t", arr[i][j]);
        }
        printf("\n");
    }
}


double computeDFD_new(int startx, int endx, int starty, int endy)
{
    if (mem[endx][endy] > -1)
        return mem[endx][endy];
    // if top left column, just compute the distance
    else if (endx == startx && endy == starty)
        mem[endx][endy] = euclideanDistance(p->pts[endx], q->pts[endy]);
    // can either be the actual distance or distance pulled from above
    else if (endx > startx && endy == starty)
        mem[endx][endy] = max(computeDFD_new(startx, endx - 1, starty, endy), euclideanDistance(p->pts[endx], q->pts[endy]));
    // can either be the distance pulled from the left or the actual
    // distance
    else if (endx == startx && endy > starty)
        mem[endx][endy] = max(computeDFD_new(startx, endx, starty ,endy - 1), euclideanDistance(p->pts[endx], q->pts[endy]));
    // can be the actual distance, or distance from above or from the left
    else if (endx > startx && endy > starty) {
        double temp = min(min(computeDFD_new(startx, endx - 1, starty ,endy), computeDFD_new(startx, endx - 1,starty,  endy - 1)), computeDFD_new(startx, endx, starty, endy - 1));
        mem[endx][endy] = max(temp, euclideanDistance(p->pts[endx], q->pts[endy]));
    }
    // infinite
    else
        mem[endx][endy] = 10000;

    // printMemory();
    // return the DFD
    return mem[endx][endy];
}

void initP_Q(Sequence *m, Sequence *n)
{
    p = m;
    q = n;
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


int getZoom(QVector<Sequence> seqV)
{
    double maxLon = seqV[0].getMaxX();
    double maxLat = seqV[0].getMaxY();
    double minLon = seqV[0].getMinX();
    double minLat = seqV[0].getMinY();
    for (int i = 1;i<seqV.length();i++)
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
    double res = getDistance(minLon, minLat, maxLon, maxLat);\
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
    return 6;
}


double rad(double d)
{
    return d * PI / 180.0;
}


int getZoom(Sequence seq_a)
{
    double maxLon = seq_a.getMaxX();
    double maxLat = seq_a.getMaxY();
    double minLon = seq_a.getMinX();
    double minLat = seq_a.getMinY();
    double res = getDistance(minLon, minLat, maxLon, maxLat);\
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
    return 6;
}
