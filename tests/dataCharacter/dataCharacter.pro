include(../TestUnitRolisteam.pri)

QT       += testlib

QT       -= gui

TARGET = tst_datacharactertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += $$ROLISTEAMSRC/src/data/charactersheet.h

SOURCES += tst_datacharactertest.cpp \
    $$ROLISTEAMSRC/src/data/charactersheet.cpp

