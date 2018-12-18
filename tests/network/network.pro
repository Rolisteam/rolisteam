include(../tests.pri)

SOURCES += \
    tst_network.cpp \
    $$ROLISTEAMSRC/network/networkmessagewriter.cpp\
    $$ROLISTEAMSRC/network/networkmessage.cpp\
    $$ROLISTEAMSRC/network/passwordaccepter.cpp \
    $$ROLISTEAMSRC/network/ipbanaccepter.cpp \
    $$ROLISTEAMSRC/network/iprangeaccepter.cpp \
    $$ROLISTEAMSRC/network/timeaccepter.cpp \
    $$ROLISTEAMSRC/network/connectionaccepter.cpp \



HEADERS += $$ROLISTEAMSRC/network/networkmessagewriter.h \
        $$ROLISTEAMSRC/network/networkmessage.h \
        $$ROLISTEAMSRC/network/passwordaccepter.h \
        $$ROLISTEAMSRC/network/ipbanaccepter.h \
        $$ROLISTEAMSRC/network/iprangeaccepter.h \
        $$ROLISTEAMSRC/network/timeaccepter.h \
        $$ROLISTEAMSRC/network/connectionaccepter.h \

TEMPLATE = app
TARGET=networkTest
MOC_DIR=trash
OBJECTS_DIR=trash

QT += testlib
CONFIG += testcase

INCLUDEPATH += ../network/
INCLUDEPATH += ../$$ROLISTEAMSRC/network/
INCLUDEPATH += ../$$ROLISTEAMSRC/

