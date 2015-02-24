include(../TestUnitRolisteam.pri)

QT       += testlib


TARGET = tst_dataplayertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
HEADERS += $$ROLISTEAMSRC/src/data/player.h \
            $$ROLISTEAMSRC/src/data/character.h \
            $$ROLISTEAMSRC/src/data/person.h

SOURCES += tst_dataplayertest.cpp \
            $$ROLISTEAMSRC/src/data/player.cpp \
            $$ROLISTEAMSRC/src/data/character.cpp \
            $$ROLISTEAMSRC/src/data/person.cpp

