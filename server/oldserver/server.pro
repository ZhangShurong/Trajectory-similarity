#-------------------------------------------------
#
# Project created by QtCreator 2016-08-12T23:47:31
#
#-------------------------------------------------

QT       += core gui widgets sql network

TARGET = tcptest
CONFIG   += console
CONFIG   += c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    server.cpp \
    ../main/csv.cpp \
    ../main/database.cpp \
    ../main/sequence.cpp \
    ../main/core.cpp \
    clientsocket.cpp 
HEADERS += \
    server.h \
    clientsocket.h 
INCLUDEPATH += ../main
