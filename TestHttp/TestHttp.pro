#-------------------------------------------------
#
# Project created by QtCreator 2017-05-06T16:40:32
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestHttp
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    myhttp.cpp \
    log.cpp

HEADERS  += mainwindow.h \
    myhttp.h \
    log.h

FORMS    += mainwindow.ui
