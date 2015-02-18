TEMPLATE = lib \
    plugin
CONFIG += plugin

include(../../../src/dicepluginmanager/dicepluginmanager.pri)

INCLUDEPATH += ../../../src \
    ../../../src/dicepluginmanager

TARGET = dicescripter
DESTDIR = ../../../bin/plugins

HEADERS += \
    scriptdicesystem.h

SOURCES += \
    scriptdicesystem.cpp
