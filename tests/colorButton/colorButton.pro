include(../tests.pri)

TARGET = tst_colorButton
CONFIG   += console
CONFIG   -= app_bundle

QT += widgets gui

TEMPLATE = app

HEADERS += $$ROLISTEAMSRC/common/widgets/colorbutton.h

SOURCES += \
    tst_colorButton.cpp \
    $$ROLISTEAMSRC/common/widgets/colorbutton.cpp

INCLUDEPATH += $$ROLISTEAMSRC/common/widgets/
#INCLUDEPATH += ../$$ROLISTEAMSRC/src/data/
#INCLUDEPATH += ../$$ROLISTEAMSRC/src/
