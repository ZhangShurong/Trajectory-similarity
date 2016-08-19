#-------------------------------------------------
#
# Project created by QtCreator 2016-03-18T13:22:16
#
#-------------------------------------------------

QT       += core gui widgets sql network

TARGET = test

TEMPLATE = app


SOURCES += test.cpp \
    ../main/csv.cpp \
    ../main/database.cpp \
    ../main/sequence.cpp \
    ../main/core.cpp \
    testqdatastream.cpp


HEADERS += \ 
    testqdatastream.h
INCLUDEPATH += ../main
