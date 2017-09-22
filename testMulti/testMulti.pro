QT += core network
QT -= gui

CONFIG += c++11

TARGET = testMulti
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    multi.cpp

HEADERS += \
    multi.h
