include(../tests.pri)

SOURCES += \
    tst_dice.cpp

TEMPLATE = app
TARGET= tst_diceParser
MOC_DIR=trash
OBJECTS_DIR=trash



QT += testlib quick qml widgets

INCLUDEPATH += ../lib/diceparser/
INCLUDEPATH += ../lib/diceparser/node
INCLUDEPATH += ../lib/diceparser/result


include(../../client/diceparser/diceparser.pri)

HEADERS -= ../../../rolisteam/client/diceparser/qmltypesregister.h
SOURCES -= ../../../rolisteam/client/diceparser/qmltypesregister.cpp


