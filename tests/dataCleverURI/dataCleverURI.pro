include(../tests.pri)


TARGET = tst_datacleveruritest
CONFIG   += console testcase
CONFIG   -= app_bundle

TEMPLATE = app


HEADERS += $$ROLISTEAMSRC/data/cleveruri.h \
    $$ROLISTEAMSRC/preferences/preferencesmanager.h \
    $$ROLISTEAMSRC/data/resourcesnode.h \

SOURCES += \
    tst_datacleveruritest.cpp \
    $$ROLISTEAMSRC/preferences/preferencesmanager.cpp \
    $$ROLISTEAMSRC/data/cleveruri.cpp \
    $$ROLISTEAMSRC/data/resourcesnode.cpp \

INCLUDEPATH += $$ROLISTEAMSRC/data/ \
                $$ROLISTEAMSRC/preferences/ \
                $$ROLISTEAMSRC/ \


