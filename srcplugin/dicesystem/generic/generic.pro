TEMPLATE = lib \
    plugin
CONFIG += plugin
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
include(../../../src/dicepluginmanager/dicepluginmanager.pri)

INCLUDEPATH += ../../../src \
    ../../../src/dicepluginmanager

TARGET = generic
DESTDIR = ../../../bin/plugins

HEADERS += \
    genericdicesystem.h

SOURCES += \
    genericdicesystem.cpp
