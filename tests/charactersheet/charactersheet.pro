include(../tests.pri)

SOURCES += \
    tst_charactersheet.cpp\

TEMPLATE = app
TARGET= tst_charactersheet
MOC_DIR=trash
OBJECTS_DIR=trash



QT += testlib widgets gui
CONFIG += UNIT_TEST

INCLUDEPATH += $$ROLISTEAMSRC/
INCLUDEPATH += $$ROLISTEAMSRC/charactersheet/nodes
INCLUDEPATH += $$ROLISTEAMSRC/charactersheet


include(../../client/charactersheet/charactersheet.pri)
HEADERS -= ../../rolisteam/client/diceparser/qmltypesregister.h
SOURCES -= ../../rolisteam/client/diceparser/qmltypesregister.cpp
