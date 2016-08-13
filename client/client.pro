#-------------------------------------------------
#
# Project created by QtCreator 2016-08-12T23:49:47
#
#-------------------------------------------------

QT       += core gui widgets sql network
TARGET = tcpclient
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \
    ../main/csv.cpp \
    ../main/database.cpp \
    ../main/sequence.cpp \
    ../main/core.cpp \
    client.cpp

HEADERS += \
    client.h
INCLUDEPATH += ../main
