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
        drawSqu(&se_draw[i]);
    }
    createFunc();
}

void MapWindow::drawSequences(QVector<Sequence> se_draw_a)
{
    markList.clear();
    labelList.clear();
    for (int i = 0; i< se_draw_a.length(); i++)
    {
        drawSqu(&(se_draw_a[i]), i);
    }
    createFunc();
}

void MapWindow::drawSequence(Sequence se_a, int c, int lWeight)
{
    if (se_a.getNum() == 0)
    {
        setCentralPoint(116.3,39.9,5);
        return;
    }
    markList.clear();
    labelList.clear();

    QString ID = "t" + se_a.getID();//如t1_40

    QString str = "var "+ ID + " = new BMap.Polyline([";
    for (int i = 0;i<se_a.getNum();i++)
    {
        str = str + "new BMap.Point("
                  + QString::number(se_a.pts[i].longitude)
                  + ", "
                  + QString::number(se_a.pts[i].latitude)
                  + "),";
    }
    c = c%color.length();//防止颜色超界

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
        c = c % pointColor.length();
        //qDebug() << color;
        out << "var "
            << "icon"+se_a.getID()    //icon1_40
            << "= new BMap.Icon(\"./"
            << pointColor[c]
            <<"\", new BMap.Size(10,10));\n";
        out.flush();
        drawPoints(se_a.pts, se_a.getNum(), se_a.getID());
    }
    createFunc();
}

void MapWindow::initMap()
{
    QTextStream out(jsFile);
    out << "var map = new BMap.Map(\"allmap\");\n"
        << "map.enableScrollWheelZoom(true);\n";
    out.flush();
    view->reload();
}

void MapWindow::drawPoints(Point *ps_a, int num, QString sqeID)
{
    QTextStream out(jsFile);

    for (int i = 0; i < num; i++)
    {
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
            <<"icon"+sqeID
             << "});"
            <<"map.addOverlay("
            << "marker"+pid
            <<");\n";
        out.flush();
        //至此，已画上一个点
        markList << "marker" + pid;

        if (ps_a[i].time != "" && showTime)//三维数据且显示时间为真
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
    color << "#6C3365"
          <<"red"
          <<"blue"
          <<"black"
          <<"yellow"
          <<"pink"
          <<"green"
         << "#4EFEB3";
    pointColor << "6C3365.png"
               << "red.png"
               << "skyblue.png"
               << "black.png"
               << "yellow.png"
               << "pink.png"
               << "green.png"
               << "red.png"
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
     out <<"var point = new BMap.Point("
         <<QString::number(x)
         <<","+QString::number(y)+");\n"
         <<"map.centerAndZoom(point,"
         << zoom
         <<");\n";
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

void MapWindow::reload()
{
    view->reload();
}

void MapWindow::drawPoint(Point *p_a, QString pid, int color)
{
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
        //qDebug() << color;
        out << "var "
            << "icon"+pid
            << "= new BMap.Icon(\"./"
            << pointColor[color]
            <<"\", new BMap.Size(10,10));\n";

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

void MapWindow::showPoints(bool sp_a)
{
    showPoint = sp_a;
}

void MapWindow::showTimes(bool st_a)
{
    showTime = st_a;
}

void MapWindow::highLightPart(Sequence *se_a, int start, int end, int c, int lWeight)
{
    QString ID = "t" + se_a->getID();
    QString str = "var "+ ID + " = new BMap.Polyline([";
    for (int i = start;i <= end;i++)
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
        setCentralPoint(116.3,39.9,5);
        return;
    }
    QString ID = "t" + se_a->getID();//如t1_40

    QString str = "var "+ ID + " = new BMap.Polyline([";
    for (int i = 0;i<se_a->getNum();i++)
    {
        str = str + "new BMap.Point("
                  + QString::number(se_a->pts[i].longitude)
                  + ", "
                  + QString::number(se_a->pts[i].latitude)
                  + "),";
    }
    c = c%color.length();//防止颜色超界

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
        c = c % pointColor.length();
        //qDebug() << color;
        out << "var "
            << "icon"+se_a->getID()    //icon1_40
            << "= new BMap.Icon(\"./"
            << pointColor[c]
            <<"\", new BMap.Size(10,10));\n";
        out.flush();
        drawPoints(se_a->pts, se_a->getNum(), se_a->getID());
    }
}

void MapWindow::createFunc()
{
    QTextStream out(jsFile);
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
    htmlFilePath = "./demo.html";
    jsFilePath = "./main.js";
    se_draw = new Sequence();
    showPoint = false;
    showTime = false;
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
    draw();
    editJs();
}

void MapWindow::draw()
{
    view->load(QUrl::fromLocalFile(QFileInfo(htmlFilePath).absoluteFilePath()));
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

void MapWindow::clearVarList()
{
    labelList.clear();
    markList.clear();
}
