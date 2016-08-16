#include "sequence.h"
#include "core.h"

int Sequence::getNumByID()
{
    QStringList list = id.split("_");
    QString s = list[0];
    QString e = list[1];
    return e.toInt() - s.toInt();
}

Sequence::Sequence(QString ID)
{
    seek = 0;
    id = ID;
    pointsNum = getNumByID();
    initSize = pointsNum;
    pts = new Point[pointsNum];
}


Sequence::Sequence()
{
    seek = 0;
    id = "";
    pointsNum = 0;
    initSize = 15;
    pts = new Point[initSize];
}

void Sequence::initPainted()
{
    for(int i = 0; i < pointsNum; i++)
    {
        pts[i].painted = false;
    }
}

void Sequence::clear()
{
    if (pointsNum != 0)
    {
        delete[] pts;
        seek = 0;
        id = "";
        pointsNum = 0;
        initSize = 15;
        pts = new Point[initSize];
    }
}

void Sequence::appendPt(Point *pt)
{
    if (seek == initSize)
    {
        initSize *=2;
        Point *temp = new Point[initSize];
        for (int i = 0; i < seek; i++)
        {
            temp[i] = pts[i];
        }
        delete []pts;
        pts = temp;
    }
    pts[seek] = *pt;
    seek++;
    pointsNum = seek;

}

void Sequence::setPtNum(int num)
{
    pointsNum = num;
}

void Sequence::setPoints(Point *points)
{
    Point *temp = pts;
    delete []temp;
    pts = points;
    seek = pointsNum;
}

void Sequence::outputTrac()
{
    for (int i = 0; i < pointsNum; i++)
    {
        qDebug() << "x = "
                 << pts[i].latitude
                 << "\n y= "
                 << pts[i].longitude
                 << endl;
    }
}

int Sequence::getNum()
{
    return pointsNum;
}

void Sequence::setID(QString ID)
{
    id = ID;
}

QString Sequence::getID()
{
    return id;
}

Point Sequence::getCentralPoint()
{
    double maxX = pts[0].longitude;
    double maxY = pts[0].latitude;
    double minX = maxX;
    double minY = maxY;
    for (int i = 0; i < pointsNum; i++)
    {
        if (pts[i].longitude > maxX)
            maxX = pts[i].longitude;
        else if (pts[i].longitude < minX)
            minX = pts[i].longitude;
        if (pts[i].latitude > maxY)
            maxY = pts[i].latitude;
        else if (pts[i].latitude < minY)
            minY = pts[i].latitude;
    }
    Point temp;
    temp.longitude = ((minX+maxX)/2);
    temp.latitude = ((minY+maxY)/2);
    return temp;
}

double Sequence::getMaxX()
{
    if (pointsNum == 0)
        return 0;
    double res = pts[0].longitude;
    for (int i = 0; i< pointsNum; i++)
    {
        res = pts[i].longitude > res?pts[i].longitude:res;
    }
    return res;
}

double Sequence::getMaxY()
{

    if (pointsNum == 0)
        return 0;
    double res = pts[0].latitude;
    for (int i = 0; i< pointsNum; i++)
    {
        res = pts[i].latitude > res?pts[i].latitude:res;
    }
    return res;
}

double Sequence::getMinX()
{
    if (pointsNum == 0)
        return 0;
    double res = pts[0].longitude;
    for (int i = 0; i< pointsNum; i++)
    {
        res = pts[i].longitude < res?pts[i].longitude:res;
    }
    return res;
}

double Sequence::getMinY()
{
    if (pointsNum == 0)
        return 0;
    double res = pts[0].latitude;
    for (int i = 0; i< pointsNum; i++)
    {
        res = pts[i].latitude < res?pts[i].latitude:res;
    }
    return res;
}

int Sequence::getIndex(Point pt)
{
    for (int i = 0; i < pointsNum; i++)
    {
        if (pt.id == pts[i].id)
            return i;
    }
    return -1;
}

bool Sequence::hasTime()
{
    if (pointsNum <= 0)
        return false;
    else
    {
        if (pts[0].time != "")
        {
            return true;
        }
        else
        {
            return false;
        }
    }

}


void Point::buildPoint(QString pt, QString te, QString ID)
{
    QStringList temp = pt.split(" ");
    QString x = temp[0];
    QString y = temp[1];
    longitude = x.remove(0,6).toDouble();
    //latitude = y.remove(y.length()-1,1).toDouble();
    latitude = y.toDouble();
    time = te;
    id = ID;
    //qDebug() << QString::number(longitude) << "   "<< QString::number(latitude) << "\n";
}


Point::Point()
{
    longitude = -1;
    latitude = -1;
    time = "";
    id = "";
    painted=false;
}

Point::Point(double x, double y, QString time_a, QString id_a)
{
    longitude = x;
    latitude = y;
    time = time_a;
    id = id_a;
    painted=false;
}
double Sequence::getPrefixSum(int i)
{
    if(prefixSum.size() <= pointsNum)
    {
        buildPrefixSum();
    }
    return prefixSum[i];
}
void Sequence::buildPrefixSum()
{
    if(pointsNum <= 0)
    {
        return;
    }
    prefixSum.push_back(0);
    for(int i = 0; i < pointsNum; i++)
    {
        prefixSum.push_back(prefixSum[prefixSum.size() - 1] +
                euDistance(pts[prefixSum.size() - 1], pts[prefixSum.size()]));
    }
}


/*
Point::Point(Point &p)
{
    p.id = id;
    p.latitude = latitude;
    p.longitude = longitude;
    p.painted = painted;;
    p.time = time;
    p.t = t;
}
*/
