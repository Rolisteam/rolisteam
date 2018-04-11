include(../UnitTestRolisteam.pri)

TEMPLATE = app

QT       += testlib

MOC_DIR=trash
OBJECTS_DIR=trash

TARGET = tst_picturetest
CONFIG   += console
CONFIG   -= app_bundle


HEADERS +=  ../$$ROLISTEAMSRC/src/Image.h \
            ../$$ROLISTEAMSRC/src/widgets/colorselector.h \
            ../$$ROLISTEAMSRC/src/data/cleveruri.h \
            ../$$ROLISTEAMSRC/src/widgets/colortablechooser.h

SOURCES +=  ../$$ROLISTEAMSRC/src/Image.cpp \
            ../$$ROLISTEAMSRC/src/widgets/colorselector.cpp \
            ../$$ROLISTEAMSRC/src/data/cleveruri.cpp \
            ../$$ROLISTEAMSRC/src/widgets/colortablechooser.cpp \
            tst_picturetest.cpp
