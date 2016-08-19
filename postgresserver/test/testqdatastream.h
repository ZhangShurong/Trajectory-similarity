#ifndef TESTQDATASTREAM_H
#define TESTQDATASTREAM_H
#include <QDataStream>
#include <QtNetWork>
class TestQdataStream
{
public:
    TestQdataStream();
    int getSum();
    int a;
    int b;
    int c;
    friend QDataStream &operator >>(QDataStream &in, TestQdataStream &t)
    {
        in >> t.a >> t.b >> t.c;
        return in;
    }

    friend QDataStream &operator <<(QDataStream &out, TestQdataStream &t)
    {
        out << t.a << t.b << t.c;
        return out;
    }

};


#endif // TESTQDATASTREAM_H
