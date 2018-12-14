include(../tests.pri)

TEMPLATE = app

QT       += testlib widgets network

MOC_DIR=trash
OBJECTS_DIR=trash

TARGET = tst_picturetest
CONFIG   += console testcase
CONFIG   -= app_bundle


HEADERS +=  $$ROLISTEAMSRC/image.h \
$$ROLISTEAMSRC/widgets/onlinepicturedialog.h \
            $$ROLISTEAMSRC/data/cleveruri.h \
    $$ROLISTEAMSRC/preferences/preferencesmanager.h \
    $$ROLISTEAMSRC/data/resourcesnode.h \
$$ROLISTEAMSRC/data/mediacontainer.h \
$$ROLISTEAMSRC/network/networkmessagewriter.h \
$$ROLISTEAMSRC/network/networkmessagereader.h \
$$ROLISTEAMSRC/network/networkmessage.h \

SOURCES +=  $$ROLISTEAMSRC/image.cpp \
            $$ROLISTEAMSRC/widgets/onlinepicturedialog.cpp \
            $$ROLISTEAMSRC/data/cleveruri.cpp \
    $$ROLISTEAMSRC/preferences/preferencesmanager.cpp \
$$ROLISTEAMSRC/data/mediacontainer.cpp \
$$ROLISTEAMSRC/network/networkmessagewriter.cpp \
$$ROLISTEAMSRC/network/networkmessagereader.cpp \
$$ROLISTEAMSRC/network/networkmessage.cpp \
    $$ROLISTEAMSRC/data/resourcesnode.cpp \
            tst_picturetest.cpp

INCLUDEPATH += $$ROLISTEAMSRC/ \
            $$ROLISTEAMSRC/data/

FORMS += $$ROLISTEAMSRC/widgets/onlinepicturedialog.ui \
