include(../tests.pri)

TARGET = tst_rolisteamtheme
CONFIG   += console
CONFIG   -= app_bundle

QT += widgets gui

TEMPLATE = app

HEADERS += $$ROLISTEAMSRC/preferences/rolisteamtheme.h

SOURCES += \
    tst_rolisteamtheme.cpp \
    $$ROLISTEAMSRC/preferences/rolisteamtheme.cpp

INCLUDEPATH += $$ROLISTEAMSRC/preferences/
INCLUDEPATH += $$ROLISTEAMSRC/data/
INCLUDEPATH += $$ROLISTEAMSRC/

QT += widgets gui
QT += testlib
