include(../tests.pri)
#include($$ROLISTEAMSRC/src/data/data.pri)

CONFIG += testcase
QT += widgets
TEMPLATE = app
HEADERS = $$ROLISTEAMSRC/data/chapter.h \
          $$ROLISTEAMSRC/data/cleveruri.h \
          $$ROLISTEAMSRC/data/resourcesnode.h \
        $$ROLISTEAMSRC/data/character.h \
$$ROLISTEAMSRC/data/characterstate.h \
$$ROLISTEAMSRC/data/person.h \
$$ROLISTEAMSRC/data/player.h \
$$ROLISTEAMSRC/network/networkmessagewriter.h \
$$ROLISTEAMSRC/network/networkmessagereader.h \
$$ROLISTEAMSRC/network/networkmessage.h \
          $$ROLISTEAMSRC/preferences/preferencesmanager.h \



SOURCES += \
    testChapter.cpp\
    $$ROLISTEAMSRC/data/cleveruri.cpp \
    $$ROLISTEAMSRC/data/chapter.cpp\
    $$ROLISTEAMSRC/data/resourcesnode.cpp\
$$ROLISTEAMSRC/data/character.cpp \
$$ROLISTEAMSRC/data/characterstate.cpp \
$$ROLISTEAMSRC/data/person.cpp \
$$ROLISTEAMSRC/data/player.cpp \
$$ROLISTEAMSRC/network/networkmessagewriter.cpp \
$$ROLISTEAMSRC/network/networkmessagereader.cpp \
$$ROLISTEAMSRC/network/networkmessage.cpp \
$$ROLISTEAMSRC/preferences/preferencesmanager.cpp \

TARGET=data
MOC_DIR=trash
OBJECTS_DIR=trash






