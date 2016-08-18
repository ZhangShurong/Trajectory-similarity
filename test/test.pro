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
INCLUDEPATH += ../main\
              ./3rdParty/include/opencv\
              $$PWD/3rdParty/include/opencv2\
              $$PWD/3rdParty/include

LIBS += $$PWD/3rdParty/lib/libopencv_calib3d310.dll.a\
        $$PWD/3rdParty\lib\libopencv_core310.dll.a\
        $$PWD/3rdParty\lib\libopencv_features2d310.dll.a\
        $$PWD/3rdParty\lib\libopencv_flann310.dll.a\
        $$PWD/3rdParty\lib\libopencv_imgcodecs310.dll.a\
        $$PWD/3rdParty\lib\libopencv_highgui310.dll.a\
        $$PWD/3rdParty\lib\libopencv_imgproc310.dll.a\
        $$PWD/3rdParty\lib\libopencv_ml310.dll.a\
        $$PWD/3rdParty\lib\libopencv_objdetect310.dll.a\
        $$PWD/3rdParty\lib\libopencv_video310.dll.a
