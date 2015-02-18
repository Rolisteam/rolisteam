TEMPLATE = lib \
    plugin
CONFIG += plugin
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
include(../../../src/dicepluginmanager/dicepluginmanager.pri)

INCLUDEPATH += ../../../src \
    ../../../src/dicepluginmanager

TARGET = dicescripter
DESTDIR = ../../../bin/plugins

HEADERS += \
    scriptdicesystem.h

SOURCES += \
    scriptdicesystem.cpp
