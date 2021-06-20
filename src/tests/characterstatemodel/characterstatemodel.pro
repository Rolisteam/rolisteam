include(../tests.pri)

TEMPLATE = app
TARGET=character_state_model_test
MOC_DIR=trash
OBJECTS_DIR=trash
CONFIG+=testcase

QT += testlib


SOURCES += tst_characterstatemodel.cpp \


# characterstate
SOURCES += $$ROLISTEAMSRC/preferences/characterstatemodel.cpp
HEADERS += $$ROLISTEAMSRC/preferences/characterstatemodel.h


#Others
SOURCES += $$ROLISTEAMSRC/preferences/preferencesmanager.cpp\
$$ROLISTEAMSRC/preferences/preferenceslistener.cpp\
$$ROLISTEAMSRC/data/characterstate.cpp \
    $$ROLISTEAMSRC/network/networkmessagewriter.cpp \
    $$ROLISTEAMSRC/network/networkmessagereader.cpp \
    $$ROLISTEAMSRC/data/character.cpp \
    $$ROLISTEAMSRC/data/person.cpp \
    $$ROLISTEAMSRC/data/resourcesnode.cpp \
    $$ROLISTEAMSRC/network/networkmessage.cpp \
    $$ROLISTEAMSRC/data/cleveruri.cpp \
    $$ROLISTEAMSRC/data/player.cpp \

HEADERS += $$ROLISTEAMSRC/preferences/preferencesmanager.h\
$$ROLISTEAMSRC/data/characterstate.h \
$$ROLISTEAMSRC/preferences/preferenceslistener.h\
    $$ROLISTEAMSRC/network/networkreceiver.h \
    $$ROLISTEAMSRC/network/networkmessagewriter.h \
    $$ROLISTEAMSRC/network/networkmessagereader.h \
    $$ROLISTEAMSRC/data/character.h \
    $$ROLISTEAMSRC/data/person.h \
    $$ROLISTEAMSRC/data/resourcesnode.h \
    $$ROLISTEAMSRC/network/networkmessage.h \
    $$ROLISTEAMSRC/data/cleveruri.h \
    $$ROLISTEAMSRC/data/player.h \
