include(../tests.pri)

SOURCES += \
    tst_dice.cpp

TEMPLATE = app
TARGET= tst_diceParser
MOC_DIR=trash
OBJECTS_DIR=trash

CONFIG+=testcase

QT += testlib quick qml widgets

INCLUDEPATH += $$ROLISTEAMSRC/diceparser/
INCLUDEPATH += $$ROLISTEAMSRC/diceparser/node
INCLUDEPATH += $$ROLISTEAMSRC/diceparser/result


include($$ROLISTEAMSRC/diceparser/diceparser.pri)

HEADERS -= ../../../rolisteam/client/diceparser/qmltypesregister.h
SOURCES -= ../../../rolisteam/client/diceparser/qmltypesregister.cpp


