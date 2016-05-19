#include "core.h"
#include <QDebug>
#include <QtAlgorithms>
#include<time.h>
#include <calwindow.h>

using namespace std;
double **mem;
Sequence *p;
Sequence *q;
double coef;


double computeDiscreteFrechet(Sequence *sa, Sequence *sb)
{
    p = sa;
    q = sb;
    coef=calCoef();
    initMemSpace(sa, sb);
    return computeDFD(sa->getNum()-1,sb->getNum()-1,p,q);
}
//int i=0;
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
    string line;
    bool ok;
    QString xStr;
    QString yStr;
    double x;
    double y;
    QTextStream cout(stdout,  QIODevice::WriteOnly);
    QVector<Point*> tContainer;
    while (csv->getline(line) != 0) {
        const char *t = line.c_str();
        if (!(t[0]>=0 && t[0]<=127))
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
                  //  se->appendPt(temp);
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
            cout<<"beign "<<tContainer[i]->time<<endl;
        }
    }
       tContainer.clear();
}


Time& loadToStruct(QString time){
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
}

void initMemSpace(Sequence *p, Sequence *q)
{
    Sequence *m;

    if(p->getNum()>=q->getNum()){
          m = p;
      }else{
          m=q;
      }
       mem = new double*[m->getNum()];
       for (int k = 0; k < m->getNum(); k++)
       {
           mem[k] = new double[m->getNum()];
       }
       for (int i = 0; i < m->getNum(); i++) {
                   for (int j = 0; j < m->getNum(); j++) {
                       mem[i][j] = -1.0;
                   }
               }
}

//分别输入两个轨迹段的起点和终位置
double  getSecSim(int i1,int j1,int i2,int j2){
    initMemSpace(p, q);
  //if((i1>=0&&i2>=0)&&(i1<=j1&&i2<=j2)&&(j1<=p->pointsNum&&j2<=q->pointsNum)){
     QTextStream cout(stdout,  QIODevice::WriteOnly);
     Sequence m1;
     Sequence m2;

     m1.pointsNum=j1-i1+1;
     m2.pointsNum=j2-i2+1;

     cout<<m1.pointsNum<<" "<<m2.pointsNum<<endl;

      m1.pts = new Point[m1.pointsNum];
      m2.pts = new Point[m2.pointsNum];

     for(int i=i1;i<=j1;i++){
       //  cout<<"fsdfdsf"<<endl;
         m1.pts[i-i1]=p->pts[i];
     }
     for(int i=i2;i<=j2;i++){
        // cout<<"asdf"<<endl;
         m2.pts[i-i2]=q->pts[i];
     }

      return computeDFD(m1.pointsNum-1, m2.pointsNum-1,&m1,&m2);
}

QVector<SecCompare> findSimilarSection(Sequence *se_a, Sequence *se_b,int a)
{
      QVector<SecCompare> q1;
      QVector<SecCompare> q2;
      QVector<SecCompare> q3;
 //     QVector<SecCompare> q4;

      p = se_a;
      q = se_b;
      QTextStream cout(stdout,  QIODevice::WriteOnly);

       cout<<p->pointsNum<<" "<<q->pointsNum<<endl;
       int totalNum=p->pointsNum+q->pointsNum;

       int gap;
       int h;

       if(totalNum<=200&&totalNum>=40){
           if(a==1){
               gap=5;
               h=1;
           }else if(a==2){
               gap=1;
               h=1;
           }
       }else if(totalNum<40){
            gap=1;
            h=1;
       }else if(totalNum>200){
           gap=5;
           h=gap;
       }


         for(int i=gap;i<gap+3;i++){
               calculateSec(i,h,q1);
          }

        for(int j=gap;j<gap+5;j++){
               mergeChange(j,q1,q2);
        }
          sort(q2.begin(),q2.end(),compare);
          for(int i=0;i<q2.size();i++){
            // cout<<i<<" ("<<q2[i].beginIndex1<<","<<q2[i].endIndex1<<")"<<","<<"("<<q2[i].beginIndex2<<","<<q2[i].endIndex2<<")       similarity="<<q2[i].simliarity<<endl;
            q1.append(q2[i]);
            if(i>100)
                break;
          }
          sort(q1.begin(),q1.end(),compare);

          if(q1.size()>100){
              for(int i=0;i<100;i++){
                  q3.append(q1[i]);
              }
          }else{
              for(int i=0;i<q1.size();i++){
                  q3.append(q1[i]);
              }
          }
          return q3;
}

QVector<SecCompare> findBest(Sequence*p,Sequence*q,int& beginMin1,int& beginMin2){

    QVector<SecCompare> temp = getBestSce(findSimilarSection(p, q,1),1);

//    int beginMin1,beginMin2;
    beginMin1=temp[0].beginIndex1;
    int endMax1=temp[0].endIndex1;
    beginMin2=temp[0].beginIndex2;
    int endMax2=temp[0].endIndex2;

    for (int i = 0; i < temp.length(); i++)
    {
       if(temp[i].beginIndex1<=beginMin1)
           beginMin1=temp[i].beginIndex1;
       if(temp[i].beginIndex2<=beginMin2)
           beginMin2=temp[i].beginIndex2;
       if(temp[i].endIndex1>endMax1)
           endMax1=temp[i].endIndex1;
       if(temp[i].endIndex2>endMax2)
           endMax2=temp[i].endIndex2;
    }

    Sequence m1;
    Sequence m2;

    m1.pointsNum=endMax1-beginMin1+1;
    m2.pointsNum=endMax2-beginMin2+1;

    cout<<m1.pointsNum<<" "<<m2.pointsNum<<endl;

     m1.pts = new Point[m1.pointsNum];
     m2.pts = new Point[m2.pointsNum];

    for(int i=beginMin1;i<=endMax1;i++){
        m1.pts[i-beginMin1]=p->pts[i];
    }
    for(int i=beginMin2;i<=endMax2;i++){
        m2.pts[i-beginMin2]=q->pts[i];
    }

   QVector<SecCompare>qs= getBestSce(findSimilarSection(&m1,&m2,2),2);
   return qs;
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
    int t=0;
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
      t++;
    }
//        if(t>1000)
//            break;
    }
}

void mergeChange(int gap, QVector<SecCompare> &q1, QVector<SecCompare> &q2)
{
    double limit = 0.2;
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

QVector<SecCompare> getBestSce(QVector<SecCompare> secCompareV_a,int a)
{
    QVector<SecCompare> t;
   // QVector<SecCompare> t1;
    if (secCompareV_a.length() == 0)
    {
        return t;
    }
    int i;
    double similarity  = secCompareV_a[0].simliarity;
    int length = secCompareV_a[0].endIndex1 - secCompareV_a[0].beginIndex1;
   // t.append(secCompareV_a[0]);

   if(a==1){
       for (i = 0; i < secCompareV_a.length() ; i++)
       {
           if (secCompareV_a[i].simliarity > (similarity+0.001))
               break;
           if (length <= secCompareV_a[i].endIndex1 - secCompareV_a[i].beginIndex1)
           {
               length = secCompareV_a[i].endIndex1 - secCompareV_a[i].beginIndex1;
           }
       }
      QVector<QString>qst;
       for (i = 0; i < secCompareV_a.length() && secCompareV_a[i].simliarity <=similarity+0.001; i++)
       {
           if (length <= secCompareV_a[i].endIndex1 - secCompareV_a[i].beginIndex1)
           {

         QString s = QString::number(secCompareV_a[i].beginIndex1, 10)+" "+ QString::number(secCompareV_a[i].endIndex1, 10)+" "+QString::number(secCompareV_a[i].beginIndex2, 10)+" "+QString::number(secCompareV_a[i].endIndex2, 10);
                  if(!qst.contains(s)){
                       qst.append(s);
                       t.append(secCompareV_a[i]);
                  }
           }
       }
   }else if(a==2){
        QVector<QString>qst1;
       for (i = 0; i < secCompareV_a.length() && secCompareV_a[i].simliarity <=similarity+0.001; i++)
       {
           QString s = QString::number(secCompareV_a[i].beginIndex1, 10)+" "+ QString::number(secCompareV_a[i].endIndex1, 10)+" "+QString::number(secCompareV_a[i].beginIndex2, 10)+" "+QString::number(secCompareV_a[i].endIndex2, 10);
           if(!qst1.contains(s)){
                qst1.append(s);
                t.append(secCompareV_a[i]);
           }
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


void getSec(int m, int n)
{
    double **temp = new double*[m];
    for (int x = 0; x < m ;x++)
    {
        temp[x] = new double[n];
    }
    for (int i = 0; i < p->getNum() - m + 1; i++)
    {
        for (int j =0; j < q->getNum() - n + 1; j ++)
        {
            //从mem中取出一个坐标为(i,j)到(i + m - 1; j + n - 1)的矩阵

        }
    }
}
