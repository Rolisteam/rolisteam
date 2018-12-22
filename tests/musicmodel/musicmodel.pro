include(../tests.pri)

TEMPLATE = app
TARGET=mapTest
MOC_DIR=trash
OBJECTS_DIR=trash
CONFIG+=testcase

QT += testlib multimedia


SOURCES += tst_musicmodel.cpp


# Music
SOURCES += $$ROLISTEAMSRC/audio/musicmodel.cpp
HEADERS += $$ROLISTEAMSRC/audio/musicmodel.h


#Others
SOURCES += $$ROLISTEAMSRC/preferences/preferencesmanager.cpp\
$$ROLISTEAMSRC/data/cleveruri.cpp \
    $$ROLISTEAMSRC/data/resourcesnode.cpp \


HEADERS += $$ROLISTEAMSRC/preferences/preferencesmanager.h\
$$ROLISTEAMSRC/data/cleveruri.h \
    $$ROLISTEAMSRC/data/resourcesnode.h \
