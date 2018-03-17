TEMPLATE = app
TARGET = bin/roliserver
LANGUAGE = C++

DEPENDPATH += . ../client/
INCLUDEPATH += . ../client/

OBJECTS_DIR = obj
MOC_DIR = moc
UI_DIR = ui

DEFINES += UNIT_TEST

isEmpty(PREFIX) {
 PREFIX = /usr/local/bin
}
CONFIG += c++11

include(server.pri)

QT += core network
#gui opengl

## Translation
TRANSLATIONS =  ../translations/roliserver_fr.ts \
                ../translations/roliserver.ts \
                ../translations/roliserver_de.ts \
                ../translations/roliserver_pt_BR.ts \
                ../translations/roliserver_hu_HU.ts \
                ../translations/roliserver_tr.ts \
                ../translations/roliserver_es.ts \


# Version
DEFINES += VERSION_MAJOR=1 VERSION_MIDDLE=9 VERSION_MINOR=0
INSTALLS += target
target.path = $$PREFIX/
