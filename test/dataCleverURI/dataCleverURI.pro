include(../TestUnitRolisteam.pri)


TARGET = tst_datacleveruritest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


HEADERS += $$ROLISTEAMSRC/src/data/cleveruri.h



SOURCES += \
    tst_datacleveruritest.cpp \
    $$ROLISTEAMSRC/src/data/cleveruri.cpp
