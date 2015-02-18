TEMPLATE = lib \
    plugin
CONFIG += plugin

include(../../../src/dicepluginmanager/dicepluginmanager.pri)

INCLUDEPATH += ../../../src \
    ../../../src/dicepluginmanager

TARGET = generic
DESTDIR = ../../../bin/plugins

HEADERS += \
    genericdicesystem.h

SOURCES += \
    genericdicesystem.cpp
