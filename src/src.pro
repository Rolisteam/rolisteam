TEMPLATE = app
CONFIG += qt \
    phonon
CONFIG += HAVE_PHONON

# CONFIG += HAVE_FMOD
# CONFIG += HAVE_NULL
QT += core \
    gui \
    network
TARGET = ../bin/rolisteam
include(preferences/preferences.pri)
include(map/map.pri)
include(drawitem/drawitem.pri)
include(data/data.pri)
include(charactersheet/charactersheet.pri)
include(userlist/userlist.pri)
include(tchat/tchat.pri)
include(dicepluginmanager/dicepluginmanager.pri)
DEPENDPATH += . \
    drawitem \
    preferences \
    data \
    userlist \
    charactersheet \
    dicepluginmanager \
    tchat \
    map
INCLUDEPATH += . \
    drawitem \
    preferences \
    userlist \
    dicepluginmanager \
    data \
    tchat \
    charactersheet \
    map
HEADERS += displaydisk.h \
    ToolBar.h \
    MinutesEditor.h \
    Image.h \
    audioplayer.h \
    MainWindow.h \
    colorselector.h \
    SelecteurDiametre.h \
    improvedworkspace.h \
    submdiwindows.h \
    connectionwizzard.h
FORMS += LecteurAudio.ui \
    connectionwizzard.ui
SOURCES += displaydisk.cpp \
    ToolBar.cpp \
    MinutesEditor.cpp \
    Image.cpp \
    main.cpp \
    MainWindow.cpp \
    colorselector.cpp \
    SelecteurDiametre.cpp \
    connectionwizzard.cpp \
    improvedworkspace.cpp \
    submdiwindows.cpp


DEFINES += PHONON
SOURCES += audioplayer.cpp
QT += phonon

HAVE_NULL { 
    DEFINES += NULL_PLAYER
    SOURCES += audioplayerunix.cpp
}
RESOURCES += ../rolisteam.qrc
documentation.path = bin/doc
macx { 
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5
    documentation.path = bin/rolisteam.app/Contents/Resources/doc
}
documentation.files = resources/doc/*
INSTALLS += documentation \
    target
VERSION = 2.0.0
UI_DIR = .
MOC_DIR = ../obj
OBJECTS_DIR = ../obj
