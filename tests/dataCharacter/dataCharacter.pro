include(../tests.pri)

QT       += testlib  widgets gui

QT       -= gui

TARGET = tst_datacharactertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += $$ROLISTEAMSRC/data/character.h \
$$ROLISTEAMSRC/data/player.h \
$$ROLISTEAMSRC/data/characterstate.h \
$$ROLISTEAMSRC/network/networkmessagewriter.h \
$$ROLISTEAMSRC/network/networkmessagereader.h \
$$ROLISTEAMSRC/network/networkmessage.h \
            $$ROLISTEAMSRC/data/person.h

SOURCES += tst_datacharactertest.cpp \
$$ROLISTEAMSRC/data/player.cpp \
    $$ROLISTEAMSRC/data/resourcesnode.cpp\
$$ROLISTEAMSRC/data/characterstate.cpp \
$$ROLISTEAMSRC/network/networkmessagewriter.cpp \
$$ROLISTEAMSRC/network/networkmessagereader.cpp \
$$ROLISTEAMSRC/network/networkmessage.cpp \
    $$ROLISTEAMSRC/data/character.cpp \
    $$ROLISTEAMSRC/data/person.cpp

