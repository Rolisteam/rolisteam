include(../tests.pri)

SOURCES += \
    tst_network.cpp \
    $$ROLISTEAMSRC/network/networkmessagewriter.cpp\
    $$ROLISTEAMSRC/network/networkmessage.cpp\

HEADERS += $$ROLISTEAMSRC/network/networkmessagewriter.h \
        $$ROLISTEAMSRC/network/networkmessage.h

TEMPLATE = app
TARGET=networkTest
MOC_DIR=trash
OBJECTS_DIR=trash

QT += testlib
CONFIG += testcase

INCLUDEPATH += ../network/
INCLUDEPATH += ../$$ROLISTEAMSRC/network/
INCLUDEPATH += ../$$ROLISTEAMSRC/

