include(../tests.pri)

TEMPLATE = app
TARGET=tst_charactersheet
MOC_DIR=trash
OBJECTS_DIR=trash
CONFIG+=testcase

QT += testlib widgets


SOURCES += \
    tst_charactersheetmodel.cpp



# Music
SOURCES += $$ROLISTEAMSRC/charactersheet/charactersheetmodel.cpp
HEADERS += $$ROLISTEAMSRC/charactersheet/charactersheetmodel.h


#Others
HEADERS +=  $$ROLISTEAMSRC/network/networkmessagewriter.h \
            $$ROLISTEAMSRC/network/networkmessagereader.h \
            $$ROLISTEAMSRC/network/networkmessage.h \


SOURCES +=     $$ROLISTEAMSRC/network/networkmessagewriter.cpp \
    $$ROLISTEAMSRC/network/networkmessagereader.cpp \
    $$ROLISTEAMSRC/network/networkmessage.cpp \


include(../../client/charactersheet/charactersheet.pri)
