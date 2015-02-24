include(../TestUnitRolisteam.pri)

TEMPLATE = app

QT       += testlib

MOC_DIR=trash
OBJECTS_DIR=trash

TARGET = tst_picturetest
CONFIG   += console
CONFIG   -= app_bundle


HEADERS +=  $$ROLISTEAMSRC/src/Image.h \
            $$ROLISTEAMSRC/src/colorselector.h \
            $$ROLISTEAMSRC/src/preferences/preferencesmanager.h \
            $$ROLISTEAMSRC/src/submdiwindows.h \
cleveruri.h \
connection.h \
            $$ROLISTEAMSRC/src/widget/colortablechooser.h

SOURCES +=  $$ROLISTEAMSRC/src/Image.cpp \
            $$ROLISTEAMSRC/src/colorselector.cpp \
            $$ROLISTEAMSRC/src/preferences/preferencesmanager.cpp \
            $$ROLISTEAMSRC/src/submdiwindows.cpp\
cleveruri.cpp \
connection.cpp \
            $$ROLISTEAMSRC/src/widget/colortablechooser.cpp \
            tst_picturetest.cpp
