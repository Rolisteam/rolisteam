include(../tests.pri)

TEMPLATE = app
TARGET=mapTest
MOC_DIR=trash
OBJECTS_DIR=trash
CONFIG+=testcase

QT += testlib multimedia


SOURCES +=  tst_genericmodel.cpp \

# Music
SOURCES += $$ROLISTEAMSRC/widgets/gmtoolbox/NpcMaker/genericmodel.cpp
HEADERS += $$ROLISTEAMSRC/widgets/gmtoolbox/NpcMaker/genericmodel.h


#Others
SOURCES += $$ROLISTEAMSRC/preferences/preferencesmanager.cpp\
            $$ROLISTEAMSRC/data/person.cpp \
            $$ROLISTEAMSRC/data/player.cpp \
            $$ROLISTEAMSRC/data/character.cpp \
            $$ROLISTEAMSRC/data/characterstate.cpp \
            $$ROLISTEAMSRC/data/cleveruri.cpp \
            $$ROLISTEAMSRC/network/networkmessagewriter.cpp \
            $$ROLISTEAMSRC/network/networkmessagereader.cpp \
            $$ROLISTEAMSRC/network/networkmessage.cpp \
            $$ROLISTEAMSRC/data/resourcesnode.cpp \


HEADERS += $$ROLISTEAMSRC/preferences/preferencesmanager.h\
            $$ROLISTEAMSRC/data/person.h \
            $$ROLISTEAMSRC/data/player.h \
            $$ROLISTEAMSRC/data/character.h \
            $$ROLISTEAMSRC/data/characterstate.h \
            $$ROLISTEAMSRC/network/networkmessagewriter.h \
            $$ROLISTEAMSRC/network/networkmessagereader.h \
            $$ROLISTEAMSRC/network/networkmessage.h \
            $$ROLISTEAMSRC/data/cleveruri.h \
            $$ROLISTEAMSRC/data/resourcesnode.h \
