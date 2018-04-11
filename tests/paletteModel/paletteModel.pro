include(../tests.pri)

TARGET = tst_paletteModel
CONFIG   += console
CONFIG   -= app_bundle

QT += widgets gui

TEMPLATE = app

HEADERS += ../../client/preferences/palettemodel.h

SOURCES += \
    tst_paletteModel.cpp \
    ../../client/preferences/palettemodel.cpp

INCLUDEPATH += $$ROLISTEAMSRC/preferences/
