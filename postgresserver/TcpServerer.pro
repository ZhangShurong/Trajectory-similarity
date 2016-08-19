#-------------------------------------------------
#
# Project created by QtCreator 2013-09-16T14:24:31
#
#-------------------------------------------------

QT       += core gui widgets sql network

TARGET = QThreadTcpServer
TEMPLATE = app

QT       -= gui
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += C++11


SOURCES += main.cpp\
    tcpserver.cpp \
    tcpsocket.cpp \
    threadhandle.cpp \
       ./main/csv.cpp \
        ./main/database.cpp \
        ./main/sequence.cpp \
        ./main/core.cpp




HEADERS  += \
    $$PWD/tcpserver.h \
    $$PWD/tcpsocket.h \
    $$PWD/threadhandle.h

#unix:include($$PWD/eventdispatcher_libev/eventdispatcher_libev.pri)

INCLUDEPATH += ./main
