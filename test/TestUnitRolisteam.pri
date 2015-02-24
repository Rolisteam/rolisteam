ROLISTEAMSRC = ../../rolisteam #/home/renaud/applications/rolisteam

INCLUDEPATH = $$ROLISTEAMSRC/src \
              $$ROLISTEAMSRC/src/widget \
                $$ROLISTEAMSRC/src/charactersheet \
                $$ROLISTEAMSRC/src/data \
                $$ROLISTEAMSRC/src/dicepluginmanager \
                $$ROLISTEAMSRC/src/drawitem \
                $$ROLISTEAMSRC/src/map \
                $$ROLISTEAMSRC/src/minuteseditor \
                $$ROLISTEAMSRC/src/network \
                $$ROLISTEAMSRC/src/pdfviewer \
                $$ROLISTEAMSRC/src/preferences \
                $$ROLISTEAMSRC/src/session \
                $$ROLISTEAMSRC/src/tchat \
                $$ROLISTEAMSRC/src/userlist \
              $$ROLISTEAMSRC/bin \


DEPENDPATH = $$ROLISTEAMSRC/src \
             $$ROLISTEAMSRC/src/widget \
                $$ROLISTEAMSRC/src/charactersheet \
                $$ROLISTEAMSRC/src/data \
                $$ROLISTEAMSRC/src/dicepluginmanager \
                $$ROLISTEAMSRC/src/drawitem \
                $$ROLISTEAMSRC/src/map \
                $$ROLISTEAMSRC/src/minuteseditor \
                $$ROLISTEAMSRC/src/network \
                $$ROLISTEAMSRC/src/pdfviewer \
                $$ROLISTEAMSRC/src/preferences \
                $$ROLISTEAMSRC/src/session \
                $$ROLISTEAMSRC/src/tchat \
                $$ROLISTEAMSRC/src/userlist \
             $$ROLISTEAMSRC/bin \

DEFINES += VERSION_MAJOR=2 VERSION_MIDDLE=0 VERSION_MINOR=0





DEFINES+= PHONON
TEMPLATE=app
QT += core \
 gui \
 network \
 phonon \
 testlib
CONFIG += phonon
