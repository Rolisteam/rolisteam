include(../tests.pri)
include(../../client/widgets/gmtoolbox/NpcMaker/NpcMaker.pri)

QT       += testlib gui

TARGET = tst_fieldmodel
CONFIG   += console testcase
CONFIG   -= app_bundle

INCLUDEPATH += ../../client/widgets/gmtoolbox/NpcMaker/

TEMPLATE = app

HEADERS += $$ROLISTEAMSRC/data/character.h \
$$ROLISTEAMSRC/data/player.h \
$$ROLISTEAMSRC/data/characterstate.h \
$$ROLISTEAMSRC/network/networkmessagewriter.h \
$$ROLISTEAMSRC/network/networkmessagereader.h \
$$ROLISTEAMSRC/network/networkmessage.h \
            $$ROLISTEAMSRC/data/person.h

SOURCES += tst_fieldmodel.cpp \
$$ROLISTEAMSRC/data/player.cpp \
    $$ROLISTEAMSRC/data/resourcesnode.cpp\
$$ROLISTEAMSRC/data/characterstate.cpp \
$$ROLISTEAMSRC/network/networkmessagewriter.cpp \
$$ROLISTEAMSRC/network/networkmessagereader.cpp \
$$ROLISTEAMSRC/network/networkmessage.cpp \
    $$ROLISTEAMSRC/data/character.cpp \
    $$ROLISTEAMSRC/data/person.cpp

