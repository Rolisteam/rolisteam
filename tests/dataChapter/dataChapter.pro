include(../TestUnitRolisteam.pri)
#include($$ROLISTEAMSRC/src/data/data.pri)


TEMPLATE = app
HEADERS = $$ROLISTEAMSRC/src/data/chapter.h \
          $$ROLISTEAMSRC/src/data/cleveruri.h \
          $$ROLISTEAMSRC/src/data/ressourcesnode.h



SOURCES += \
    testChapter.cpp\
    $$ROLISTEAMSRC/src/data/cleveruri.cpp \
    $$ROLISTEAMSRC/src/data/chapter.cpp

TARGET=data
MOC_DIR=trash
OBJECTS_DIR=trash






