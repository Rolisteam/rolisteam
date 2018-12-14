include(../UnitTestRolisteam.pri)

SOURCES += \
    tst_network.cpp \
    ../$$ROLISTEAMSRC/src/network/networkmessagewriter.cpp

HEADERS += ../$$ROLISTEAMSRC/src/network/networkmessagewriter.h

TEMPLATE = app
TARGET=networkTest
MOC_DIR=trash
OBJECTS_DIR=trash

QT += testlib
CONFIG += testcase

INCLUDEPATH += ../lib/network/
INCLUDEPATH += ../$$ROLISTEAMSRC/src/network/
INCLUDEPATH += ../$$ROLISTEAMSRC/src/

#include(../lib/diceparser/diceparser.pri)
