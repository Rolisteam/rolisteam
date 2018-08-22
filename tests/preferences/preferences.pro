include(../tests.pri)

TARGET = tst_preferences
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += $$ROLISTEAMSRC/preferences/preferencesmanager.h \
    $$ROLISTEAMSRC/preferences/preferenceslistener.h \
    $$ROLISTEAMSRC/data/resourcesnode.h \


SOURCES += \
    tst_preferencestest.cpp \
    $$ROLISTEAMSRC/data/cleveruri.cpp \
    $$ROLISTEAMSRC/data/resourcesnode.cpp \
    $$ROLISTEAMSRC/preferences/preferenceslistener.cpp \
    $$ROLISTEAMSRC/preferences/preferencesmanager.cpp

INCLUDEPATH += $$ROLISTEAMSRC/preferences/
INCLUDEPATH += $$ROLISTEAMSRC/data/
INCLUDEPATH += $$ROLISTEAMSRC/
