ROLISTEAMSRC = ../../client/

DEFINES += NULL_PLAYER UNIT_TEST
CONFIG += UNIT_TEST

INCLUDEPATH = $$ROLISTEAMSRC/ \
              $$ROLISTEAMSRC/widget \
                $$ROLISTEAMSRC/charactersheet \
                $$ROLISTEAMSRC/data \
                $$ROLISTEAMSRC/dicepluginmanager \
                $$ROLISTEAMSRC/drawitem \
                $$ROLISTEAMSRC/map \
                $$ROLISTEAMSRC/minuteseditor \
                $$ROLISTEAMSRC/network \
                $$ROLISTEAMSRC/pdfviewer \
                $$ROLISTEAMSRC/preferences \
                $$ROLISTEAMSRC/session \
                $$ROLISTEAMSRC/tchat \
                $$ROLISTEAMSRC/userlist \
              $$ROLISTEAMSRC/bin \


DEPENDPATH = $$ROLISTEAMSRC \
             $$ROLISTEAMSRC/widget \
            $$ROLISTEAMSRC/charactersheet \
            $$ROLISTEAMSRC/data \
            $$ROLISTEAMSRC/dicepluginmanager \
            $$ROLISTEAMSRC/drawitem \
            $$ROLISTEAMSRC/map \
            $$ROLISTEAMSRC/minuteseditor \
            $$ROLISTEAMSRC/network \
            $$ROLISTEAMSRC/pdfviewer \
            $$ROLISTEAMSRC/preferences \
            $$ROLISTEAMSRC/session \
            $$ROLISTEAMSRC/tchat \
            $$ROLISTEAMSRC/userlist \
             $$ROLISTEAMSRC/bin \

DEFINES += VERSION_MAJOR=1 VERSION_MIDDLE=8 VERSION_MINOR=0

CONFIG += c++11

TEMPLATE=app
QT += core \
 network \
 testlib


RESOURCES += \
    $$PWD/resources.qrc
