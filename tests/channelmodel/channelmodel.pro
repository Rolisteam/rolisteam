include(../tests.pri)

TEMPLATE = app
TARGET=channelmodeltest
MOC_DIR=trash
OBJECTS_DIR=trash
CONFIG+=testcase

QT += testlib multimedia


SOURCES += tst_channelmodel.cpp


# Music
SOURCES += $$ROLISTEAMSRC/network/channelmodel.cpp
HEADERS += $$ROLISTEAMSRC/network/channelmodel.h


#Others
SOURCES += $$ROLISTEAMSRC/preferences/preferencesmanager.cpp\
    $$ROLISTEAMSRC/data/cleveruri.cpp \
    $$ROLISTEAMSRC/data/resourcesnode.cpp \
    $$ROLISTEAMSRC/network/receiveevent.cpp \
    $$ROLISTEAMSRC/network/channel.cpp\
    $$ROLISTEAMSRC/network/treeitem.cpp\
    $$ROLISTEAMSRC/network/networkmessage.cpp \
    $$ROLISTEAMSRC/network/networkmessagewriter.cpp \
    $$ROLISTEAMSRC/network/networkmessagereader.cpp \
    $$ROLISTEAMSRC/network/tcpclient.cpp \
    $$ROLISTEAMSRC/data/character.cpp \
$$ROLISTEAMSRC/data/characterstate.cpp \
    $$ROLISTEAMSRC/data/person.cpp \
    $$ROLISTEAMSRC/data/player.cpp \

HEADERS += $$ROLISTEAMSRC/preferences/preferencesmanager.h\
$$ROLISTEAMSRC/data/cleveruri.h \
    $$ROLISTEAMSRC/data/resourcesnode.h \
$$ROLISTEAMSRC/network/receiveevent.h \
$$ROLISTEAMSRC/network/channel.h\
$$ROLISTEAMSRC/network/treeitem.h\
    $$ROLISTEAMSRC/network/networkmessage.h \
    $$ROLISTEAMSRC/network/networkmessagewriter.h \
    $$ROLISTEAMSRC/network/networkmessagereader.h \
$$ROLISTEAMSRC/network/tcpclient.h \
$$ROLISTEAMSRC/data/characterstate.h \
    $$ROLISTEAMSRC/data/character.h \
    $$ROLISTEAMSRC/data/player.h \
    $$ROLISTEAMSRC/data/person.h \
