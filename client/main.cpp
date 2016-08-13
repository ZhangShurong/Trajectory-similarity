#include <QCoreApplication>
#include "client.h"
#include "core.h"
#include "csv.h"
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
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Sequence t1;
    openfile(&t1, "./data/test_1.csv");
    Sequence t2;
    openfile(&t2, "./data/test_2.csv");

    qDebug() << t1.getNum();
    Client client;
    client.sequences.push_back(t1);
    client.sequences.push_back(t2);
    client.connectToServer();
    return a.exec();
}
