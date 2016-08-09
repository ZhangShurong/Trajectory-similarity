#include "mapwindow.h"

#include <QUrl>
#include <QByteArray>
#include <QFileInfo>
#include <QFile>
/*
 * var map = new BMap.Map("allmap");
        map.enableScrollWheelZoom(true);


        var point = new BMap.Point(116.404, 39.915);
        map.centerAndZoom(point, 15);

        var polyline = new BMap.Polyline([
                new BMap.Point(116.399, 39.910),
                new BMap.Point(116.405, 39.920),
                new BMap.Point(116.425, 39.900)
        ], {strokeColor:"blue", strokeWeight:2, strokeOpacity:0.5});   //创建折线

        map.addOverlay(polyline);
 *
 */
void MapWindow::editJs()
{
    jsFile = new QFile(jsFilePath);
    if(!jsFile->open(QIODevice::WriteOnly))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }
    else
    {
        qDebug()<<"Opened"
                << jsFilePath;
    }
    initMap();
}

void MapWindow::deleteJS()
{
    jsFile->resize(0);
}

void MapWindow::initJS()
{
    if (jsFile->isOpen())
        jsFile->close();
    editJs();
    deleteJS();
    initMap();
}

void MapWindow::closeJS()
{
    jsFile->close();
    qDebug() << "Closed" << jsFilePath;
}

void MapWindow::drawSequences(Sequence *se_a, int num)
{
    se_num = num;
    se_draw = se_a;
    //初始化变量储存器
    markList.clear();
    labelList.clear();

    for (int i = 0; i < num; i++)
    {
        drawSqu(&se_draw[i], i);
    }
    createFunc();
}

void MapWindow::drawSequences(QVector<Sequence> se_draw_a)
{
    markList.clear();
    labelList.clear();
    int num = se_draw_a.length();
    if (numOfSeq != -1 && numOfSeq < num)
    {
        num  = numOfSeq;
    }
    for (int i = 0; i< num; i++)
    {
        drawSqu(&(se_draw_a[i]), i);
    }
    createFunc();
}

void MapWindow::drawSequences(QVector<Sequence> se_draw_a, QStringList coin_list)
{
    markList.clear();
    labelList.clear();
    int num = se_draw_a.length();
    if (numOfSeq != -1 && numOfSeq < num)
    {
        num  = numOfSeq;
    }
    int j = 0;
    for (int i = 0; i< se_draw_a.length(); i++)
    {
        if (coin_list.contains(se_draw_a[i].getID()))
            continue;
        drawSqu(&(se_draw_a[i]), j);
        j++;
        num--;
        if (num == 0)
        {
            break;
        }
    }
    createFunc();
}

void MapWindow::drawSequence(Sequence se_a, int c, int lWeight)
{
    markList.clear();
    labelList.clear();
    drawSqu(&se_a, c, lWeight);
    createFunc();
}

void MapWindow::drawSequencePair(Sequence *se_a, Sequence *se_b, int c)
{
    if (se_a == NULL || se_b == NULL)
    {
        cerr << "drawSequencePair: Nothing to do." << endl;
        return;
    }
    QTextStream out(jsFile);
    size_t pointsCount = se_a->getNum() > se_b->getNum() ? se_a->getNum() : se_b->getNum();
    cerr << "Total Points Counts: " << pointsCount << endl;
    c %= color.length();
    for (size_t i = 0; i < pointsCount; ++i)
    {
        out << "var cm" << i <<  "=new BMap.Polyline(["
            << "new BMap.Point(" << QString::number(se_a->pts[i].longitude)
            << "," << QString::number(se_a->pts[i].latitude) << "),"
            << "new BMap.Point(" << QString::number(se_b->pts[i].longitude)
            << "," << QString::number(se_b->pts[i].latitude) << ")],"
            << "{strokeColor:\"" + color[c] + "\", strokeWeight:1,strokeOpacity:0.5});\n"
            << "map.addOverlay(cm" << i << ");";
        out.flush();
    }
}

void MapWindow::initMap()
{
    numOfSeq = -1;
    QTextStream out(jsFile);
    out << "var map = new BMap.Map(\"allmap\");\n"
        << "map.enableScrollWheelZoom(true);\n";
    out.flush();
    view->reload();
}

void MapWindow::drawPoints(Point *ps_a, int num, QString sqeID)
{
    QTextStream out(jsFile);

    QString iconName;
    for (int i = 0; i < num; i++)
    {
        iconName = "icon"+sqeID;
        if (i ==0)
        {
            iconName = iconName + "start";
        }
        else if (i == num -1)
        {
            iconName = iconName + "end";
        }
        QString pid = "p"+QString::number(i)+sqeID; //p01_40,p11_40....p391_40
        out <<"var "
            << pid
            << " = new BMap.Point("
            << ps_a[i].longitude
            << ","
            << ps_a[i].latitude
            <<");\n"
            <<"var "
            <<"marker"+pid
            <<" = new BMap.Marker("
            << pid
            << ",{icon:"
            << iconName
            << "});"
            <<"map.addOverlay("
            << "marker"+pid
            <<");\n";
        out.flush();
        //至此，已画上一个点
        markList << "marker" + pid;
        if (i == num -2)
        {
            qDebug() << ps_a[i].time;
        }
        if (((ps_a[i]).time != "") && showTime)//三维数据且显示时间为真
            //if (!(ps_a[i].time.isNull()) && showTime)//三维数据且显示时间为真
        {
            labelList << "label"+pid;
            //var label = new BMap.Label("2016/12/12-24:00:00",{offset:new BMap.Size(-50,-20)});
            //markerp2831_60.setLabel(label);
            out << "var label"+pid
                << " = new BMap.Label(\""
                << ps_a[i].time
                <<"\",{offset:new BMap.Size(-50,-20)});\n";
            out.flush();
            out << "marker" + pid
                << ".setLabel(label" + pid
                << ")\n";
            out.flush();
            //至此，时间标签显示完毕
        }
    }
}


void MapWindow::initColor()
{
    color <<"red"
          <<"blue"
          << "#6C3365"
          <<"black"
          <<"yellow"
          <<"pink"
          <<"green"
          << "#4EFEB3";
    pointColor << "red.png"
               << "skyblue.png"
               << "6C3365.png"
               << "black.png"
               << "yellow.png"
               << "pink.png"
               << "green.png"
               << "4EFEB3.png";
}

void MapWindow::initWidget()
{
    view = new QWebView(this);
    mainLayout = new QVBoxLayout();
    mainLayout->addWidget(view);
    this->setLayout(mainLayout);
    this->setWindowTitle("Map");
}

void MapWindow::setCentralPoint(double x, double y, int zoom)
{
    QTextStream out(jsFile);
//     QString str = "var point = new BMap.Point(" + QString::number(x) + ","+QString::number(y)+");\n"
//             + "map.centerAndZoom(point," + zoom + ");\n";

    out <<"var point = new BMap.Point("
        <<QString::number(x)
        <<","+QString::number(y)+");"
        <<"map.centerAndZoom(point,"
        << zoom
        <<");";

    //out << str;
    out.flush();
}

void MapWindow::setCentralPoint(Point pt, int zoom)
{
    QTextStream out(jsFile);
    out <<"var point = new BMap.Point("
        <<QString::number(pt.longitude)
        <<","+QString::number(pt.latitude)+");\n"
        <<"map.centerAndZoom(point,"
        << zoom
        <<");\n";
    out.flush();
}

void MapWindow::setDefaultCentralPt()
{
    drawSqu(NULL);
}

void MapWindow::setNumOfSeq(int n)
{
    numOfSeq = n;
}

void MapWindow::reload()
{
    view->reload();
}

void MapWindow::drawPoint(Point *p_a, QString pid, int color, bool big)
{
    if (pid == "")
    {
        pid = "inputS";
    }
    QTextStream out(jsFile);
    if(color < 0)
    {
        out <<"var "
            << pid
            << " = new BMap.Marker(new BMap.Point("
            << p_a->longitude
            << ","
            << p_a->latitude
            <<"));\n"
            <<"map.addOverlay("
            << pid
            <<");\n";
        out.flush();
        return;
    }
    else
    {
        color = color % pointColor.length();
        QString path = "pointico/10pix/";
        QString size = "new BMap.Size(10,10));";
        if (big)
        {
            path = "pointico/15pix/";
            size = "new BMap.Size(15,15));";
        }
        //qDebug() << color;
        out << "var "
            << "icon"+pid
            << "= new BMap.Icon(\"./" + path
            << pointColor[color]
            <<"\", "
            << size
            << "\n";

        out <<"var "
            << pid
            << " = new BMap.Point("
            << p_a->longitude
            << ","
            << p_a->latitude
            <<");\n"
            <<"var "
            <<"marker"+pid
            <<" = new BMap.Marker("
            << pid
            << ",{icon:"
            <<"icon"+pid
            << "});"
            <<"map.addOverlay("
            << "marker"+pid
            <<");\n";
    }
    out.flush();
}

void MapWindow::drawPoint(Point p_a, QString pid, int color, bool big)
{
    drawPoint(&p_a,pid,color,big);
}

void MapWindow::drawPoint(Point *p_a, QString pid, int color, int num, bool big)
{
    if (pid == "")
    {
        pid = "inputS";
    }
    num = num % (10);
    QTextStream out(jsFile);
    if(color < 0)
    {
        out <<"var "
            << pid
            << " = new BMap.Marker(new BMap.Point("
            << p_a->longitude
            << ","
            << p_a->latitude
            <<"));\n"
            <<"map.addOverlay("
            << pid
            <<");\n";
        out.flush();
        return;
    }
    else
    {
        color = color % pointColor.length();
        QString path = "pointico/pointWithNum_10pix/";
        QString size = "new BMap.Size(10,10));";
        QString filename = pointColor[color];
        filename.resize(pointColor[color].length() - 4);
        filename = filename + "num" + QString::number(num) + ".png";
        if (big)
        {
            path = "pointico/pointWithNum_15pix/";
            size = "new BMap.Size(15,15));";
        }
        //qDebug() << color;
        out << "var "
            << "icon"+pid
            << "= new BMap.Icon(\"./" + path
            << filename
            <<"\", "
            << size
            << "\n";

        out <<"var "
            << pid
            << " = new BMap.Point("
            << p_a->longitude
            << ","
            << p_a->latitude
            <<");\n"
            <<"var "
            <<"marker"+pid
            <<" = new BMap.Marker("
            << pid
            << ",{icon:"
            <<"icon"+pid
            << "});"
            <<"map.addOverlay("
            << "marker"+pid
            <<");\n";
    }
    out.flush();
}

void MapWindow::drawPoints(QVector<Point> points_a, int color, bool big)
{
    for (int i = 0; i < points_a.length(); i++)
    {
        drawPoint(&(points_a[i]), points_a[i].id, color, i, big);
    }
}

void MapWindow::showPoints(bool sp_a)
{
    showPoint = sp_a;
}

void MapWindow::showTimes(bool st_a)
{
    showTime = st_a;
}

void MapWindow::showEndPoints(bool se_a)
{
    showEndpoints = se_a;
}

void MapWindow::setFilter(int mod)
{
    filterStatus = mod;
}

void MapWindow::highLightPart(Sequence *se_a, int start, int end, int c, int lWeight)
{
    QString ID = "t" + se_a->getID();
    QString str = "var "+ ID + " = new BMap.Polyline([";
    for (int i = start; i <= end; i++)
    {
        str = str + "new BMap.Point("
              + QString::number(se_a->pts[i].longitude)
              + ", "
              + QString::number(se_a->pts[i].latitude)
              + "),";
    }
    c = c%7;
    str = str + "], {strokeColor:\""
          + color[c]
          + "\","
          +" strokeWeight:"
          + QString::number(lWeight)
          + ", strokeOpacity:0.5});\n";
    str = str + "map.addOverlay("
          + ID
          + ");";
    QTextStream out(jsFile);
    out << str;
    out.flush();
}

void MapWindow::drawSqu(Sequence *se_a, int c, int lWeight)
{

    if (se_a == NULL)
    {
        setCentralPoint(116.3, 39.9, 5);
        return;
    }
    if (filterStatus == 0)
    {
        if (se_a->hasTime()) {
            return;
        }
    }
    else if(filterStatus == 1)
    {
        if (!se_a->hasTime())
        {
            return;
        }
    }

    QString ID = "t" + se_a->getID();//如t1_40
    QString str = "var "+ ID + " = new BMap.Polyline([";
    for (int i = 0; i<se_a->getNum(); i++)
    {
        str = str + "new BMap.Point("
              + QString::number(se_a->pts[i].longitude)
              + ", "
              + QString::number(se_a->pts[i].latitude)
              + "),";
    }
    c = c % color.length();//防止颜色超界

    str = str + "], {strokeColor:\""
          + color[c]
          + "\","
          +" strokeWeight:"
          + QString::number(lWeight)
          + ", strokeOpacity:0.5});\n";
    str = str + "map.addOverlay("
          + ID
          + ");";
    QTextStream out(jsFile);
    out << str;
    out.flush();
    //至此，折线已经画完
    if (showPoint)
    {
        QString path = "pointico/10pix/";
        c = c % pointColor.length();
        //qDebug() << color;
        out << "var "
            << "icon"+se_a->getID()    //icon1_40
            << "= new BMap.Icon(\"./"
            << path + pointColor[c]
            <<"\", new BMap.Size(10,10));\n";
        out.flush();


        if (showEndpoints)
        {
            QString pathForEnd = "pointico/pointWithNum_20pix/";
            QString filename = pointColor[c];
            filename.resize(pointColor[c].length() - 4);

            QString filenameS = filename + "start"+ ".png";
            QString filenameE = filename + "end"+ ".png";

            out << "var "
                << "icon"+se_a->getID() + "start"    //icon1_40start
                << "= new BMap.Icon(\"./"
                << pathForEnd + filenameS
                <<"\", new BMap.Size(20,20));\n";
            out.flush();
            out << "var "
                << "icon"+se_a->getID() + "end"    //icon1_40end
                << "= new BMap.Icon(\"./"
                << pathForEnd + filenameE
                <<"\", new BMap.Size(20,20));\n";
            out.flush();
        }
        else
        {
            QString path = "pointico/10pix/";
            out << "var "
                << "icon"+se_a->getID() + "start"    //icon1_40start
                << "= new BMap.Icon(\"./"
                << path+  pointColor[c]
                <<"\", new BMap.Size(10,10));\n";
            out.flush();
            out << "var "
                << "icon"+se_a->getID() + "end"    //icon1_40end
                << "= new BMap.Icon(\"./"
                << path + pointColor[c]
                <<"\", new BMap.Size(10,10));\n";
            out.flush();
        }
        drawPoints(se_a->pts, se_a->getNum(), se_a->getID());
    }
}

void MapWindow::createFunc()
{
    QTextStream out(jsFile);
    out << "hideOver();\n";
    out.flush();
    /*
    function showOver(){
        markerp4761_109.show(); polyline.show(); circle.show();
    }
    function hideOver(){
        markerp4761_109.hide(); polyline.hide(); circle.hide();
    }
    */

    if (markList.length() > 0)
    {
        out << "function showOver(){";
        for (int i = 0; i < markList.length(); i++)
        {
            out << markList[i]
                << ".show();\n";
        }
        out << "}\n";
        out.flush();
        out << "function hideOver(){";
        for (int i = 0; i < markList.length(); i++)
        {
            out << markList[i]
                << ".hide();\n";
        }
        out << "}\n";
        out.flush();
    }

    if (labelList.length() > 0)
    {
        out << "function showLabelOver(){";
        for (int i = 0; i < labelList.length(); i++)
        {
            out << labelList[i]
                << ".show();\n";
        }
        out << "}\n";
        out.flush();
        out << "function hideLabelOver(){";
        for (int i = 0; i < labelList.length(); i++)
        {
            out << labelList[i]
                << ".hide();\n";
        }
        out << "}\n";
        out.flush();
    }
}

MapWindow::MapWindow(QWidget *parent):QWidget(parent)
{
    initColor();
    initWidget();
    htmlFilePath = "./html/main.html";
    jsFilePath = "./html/main.js";
    se_draw = new Sequence();
    showPoint = false;
    showTime = false;
    showEndpoints = false;
    filterStatus = -1;//all
    numOfSeq = -1;
    draw();
    editJs();
}

MapWindow::MapWindow(QString js, QString html, QWidget *parent):QWidget(parent)
{
    initColor();
    initWidget();
    htmlFilePath = html;
    jsFilePath = js;
    se_draw = new Sequence();
    showPoint = false;
    showTime = false;
    showEndpoints = false;
    filterStatus = -1;//all
    numOfSeq = -1;//全部绘制
    draw();
    editJs();
}

void MapWindow::draw()
{
    view->load(QUrl::fromLocalFile(QFileInfo(htmlFilePath).absoluteFilePath()));
    qDebug() << QFileInfo(htmlFilePath).absoluteFilePath();
    //view->load(QUrl::fromLocalFile(QFileInfo(":/webmap/webmap/demo.html").absoluteFilePath()));
    //view->load(QUrl("qrc:/webmap/webmap/demo.html"));
}

MapWindow::~MapWindow()
{
    view->close();
    delete mainLayout;
    delete view;
    delete jsFile;
    delete se_draw;
}

void MapWindow::setJs(QString js)
{
    closeJS();
    jsFilePath = js;
    delete jsFile;
    editJs();
}

void MapWindow::setHtml(QString html)
{
    htmlFilePath = html;
    draw();
}

void MapWindow::clearVarList()
{
    labelList.clear();
    markList.clear();
}
