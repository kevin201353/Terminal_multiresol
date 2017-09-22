TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp \
    sysset.cpp \
    ryanConfig.cpp \
    netconfig.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    sysset.h \
    ryanConfig.h \
    netconfig.h \
    type.h
