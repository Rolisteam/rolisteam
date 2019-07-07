RCSESRC = ../../src/

DEFINES += NULL_PLAYER UNIT_TEST
CONFIG += UNIT_TEST testcase

INCLUDEPATH = $$RCSESRC/ \
              $$RCSESRC/widgets \
                $$RCSESRC/charactersheet \
                $$RCSESRC/data \
                $$RCSESRC/delegate \
                $$RCSESRC/dialog \
                $$RCSESRC/undo \
                $$RCSESRC/controllers \



DEPENDPATH = $$RCSESRC/ \
              $$RCSESRC/widgets \
                $$RCSESRC/charactersheet \
                $$RCSESRC/data \
                $$RCSESRC/delegate \
                $$RCSESRC/dialog \
                $$RCSESRC/undo \
                $$RCSESRC/controllers \


DEFINES += VERSION_MAJOR=1 VERSION_MIDDLE=8 VERSION_MINOR=0

CONFIG += c++11

TEMPLATE=app
QT += core \
 network \
 testlib


RESOURCES += \
    $$PWD/resources.qrc

SOURCES += $$PWD/helper/helper.cpp
HEADERS += $$PWD/helper/helper.h

INCLUDEPATH += $$PWD/helper
DEPENDPATH += $$PWD/helper
