#-------------------------------------------------
#
# Project created by QtCreator 2016-03-18T13:22:16
#
#-------------------------------------------------

QT       += core sql gui widgets webkitwidgets printsupport network

TARGET = main

TEMPLATE = app

QMAKE_CXXFLAGS += -fopenmp

LIBS += -lgomp -lpthread

SOURCES += main.cpp \
    csv.cpp \
    database.cpp \
    mapwindow.cpp \
    sequence.cpp \
    core.cpp \
    canvas.cpp \
    detailwin.cpp \
    mainwindow.cpp \
    calwindow.cpp \
    searchwin.cpp \
    settingwin.cpp \
    netstatindicator.cpp \
    partdetail.cpp \
    pointdetail.cpp \
    clusterdemo.cpp \
    cloud.cpp \
    client.cpp \
    lcswidget.cpp

HEADERS += \
    csv.h \
    database.h \
    mapwindow.h \
    header.h \
    sequence.h \
    core.h \
    canvas.h \
    detailwin.h \
    mainwindow.h \
    calwindow.h \
    searchwin.h \
    settingwin.h \
    netstatindicator.h \
    partdetail.h \
    pointdetail.h \
    clusterdemo.h \
    cloud.h \
    client.h \
    lcswidget.h

RESOURCES += \
    res.qrc \


FORMS += \
    detailwin.ui \
    mainwindow.ui \
    partdetail.ui \
    pointdetail.ui

DISTFILES += \
    safe.rc \
    Readme.txt

RC_FILE = safe.rc


