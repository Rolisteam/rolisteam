include(../tests.pri)

TARGET = tst_session
CONFIG   += console
CONFIG   -= app_bundle

QT += testlib widgets gui

TEMPLATE = app

HEADERS += $$ROLISTEAMSRC/session/sessionmanager.h \
    $$ROLISTEAMSRC/preferences/preferencesmanager.h \
$$ROLISTEAMSRC/data/character.h\
            $$ROLISTEAMSRC/session/sessionview.h \
$$ROLISTEAMSRC/data/cleverurimimedata.h \
$$ROLISTEAMSRC/data/characterstate.h \
$$ROLISTEAMSRC/data/person.h \
$$ROLISTEAMSRC/data/player.h \
$$ROLISTEAMSRC/network/networkmessagewriter.h \
$$ROLISTEAMSRC/network/networkmessagereader.h \
$$ROLISTEAMSRC/network/networkmessage.h \
    $$ROLISTEAMSRC/data/cleveruri.h \
    $$ROLISTEAMSRC/data/resourcesnode.h \
$$ROLISTEAMSRC/data/chapter.h \
            $$ROLISTEAMSRC/session/sessionitemmodel.h

SOURCES += \
    tst_session.cpp \
    $$ROLISTEAMSRC/session/sessionmanager.cpp \
$$ROLISTEAMSRC/preferences/preferencesmanager.cpp\
    $$ROLISTEAMSRC/data/cleveruri.cpp \
$$ROLISTEAMSRC/data/cleverurimimedata.cpp \
$$ROLISTEAMSRC/session/sessionview.cpp \
$$ROLISTEAMSRC/data/characterstate.cpp \
$$ROLISTEAMSRC/data/chapter.cpp\
$$ROLISTEAMSRC/data/character.cpp\
    $$ROLISTEAMSRC/data/resourcesnode.cpp \
$$ROLISTEAMSRC/data/person.cpp \
$$ROLISTEAMSRC/data/player.cpp \
$$ROLISTEAMSRC/network/networkmessagewriter.cpp \
$$ROLISTEAMSRC/network/networkmessagereader.cpp \
$$ROLISTEAMSRC/network/networkmessage.cpp \
$$ROLISTEAMSRC/session/sessionitemmodel.cpp

INCLUDEPATH += $$ROLISTEAMSRC/common/widgets/
