#-------------------------------------------------
#
# Project created by QtCreator 2016-03-18T13:22:16
#
#-------------------------------------------------

QT       += core sql gui widgets webkitwidgets printsupport

TARGET = importData

TEMPLATE = app


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
    lcmwidget.cpp

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
    lcmwidget.h

RESOURCES += \
    res.qrc \


FORMS += \
    detailwin.ui \
    mainwindow.ui

DISTFILES += \
    safe.rc \
    Readme.txt

RC_FILE = safe.rc
