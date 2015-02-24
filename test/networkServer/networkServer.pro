include(../TestUnitRolisteam.pri)
#include($$ROLISTEAMSRC/src/data/data.pri)


TEMPLATE = app
HEADERS = $$ROLISTEAMSRC/src/data/connection.h \
          $$ROLISTEAMSRC/src/network/server.h \
          $$ROLISTEAMSRC/src/network/rclient.h \
          $$ROLISTEAMSRC/src/data/ressourcesnode.h



SOURCES += \
    networkserver.cpp \
    $$ROLISTEAMSRC/src/network/server.cpp \
    $$ROLISTEAMSRC/src/network/rclient.cpp \
    $$ROLISTEAMSRC/src/data/connection.cpp

TARGET=networkServer
MOC_DIR=trash
OBJECTS_DIR=trash






