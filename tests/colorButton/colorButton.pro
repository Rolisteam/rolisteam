include(../tests.pri)

TARGET = tst_colorButton
CONFIG   += console
CONFIG   -= app_bundle

QT += widgets gui

TEMPLATE = app

HEADERS += ../$$ROLISTEAMSRC/widgets/colorbutton.h

SOURCES += \
    tst_colorButton.cpp \
    ../$$ROLISTEAMSRC/widgets/colorbutton.cpp

INCLUDEPATH += ../$$ROLISTEAMSRC/widgets/
#INCLUDEPATH += ../$$ROLISTEAMSRC/src/data/
#INCLUDEPATH += ../$$ROLISTEAMSRC/src/
