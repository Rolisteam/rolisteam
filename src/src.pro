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
DEPENDPATH += . \
    drawitem \
    preferences \
    data \
    charactersheet \
    map
INCLUDEPATH += . \
    drawitem \
    preferences \
    data \
    charactersheet \
    map
HEADERS += displaydisk.h \
    ToolBar.h \ # ClientServeur.h \
    DessinPerso.h \
    MinutesEditor.h \
    Image.h \
    audioplayer.h \ # Liaison.h \
    MainWindow.h \
    colorselector.h \
    SelecteurDiametre.h \
    Tchat.h \
    TextEditAmeliore.h \
    #types.h \
    #typesApplication.h \ # variablesGlobales.h \
    improvedworkspace.h \
    submdiwindows.h \
    connectionwizzard.h \
    userlistdockwidget.h
FORMS += LecteurAudio.ui \
    userlistdockwidget.ui \
    connectionwizzard.ui
SOURCES += displaydisk.cpp \
    ToolBar.cpp \ # ClientServeur.cpp \
    DessinPerso.cpp \
    MinutesEditor.cpp \
    Image.cpp \ # Liaison.cpp \
    main.cpp \
    MainWindow.cpp \
    colorselector.cpp \
    SelecteurDiametre.cpp \
    Tchat.cpp \
    TextEditAmeliore.cpp \
    connectionwizzard.cpp \
    improvedworkspace.cpp \
    submdiwindows.cpp \
    userlistdockwidget.cpp
#HAVE_FMOD {
#    DEFINES += FMOD
#    SOURCES += audioplayerwin.cpp
#    HEADERS += fmod.h \
#    fmod_errors.h
#}


HAVE_PHONON { 
    DEFINES += PHONON
    SOURCES += audioplayerunix.cpp
    QT += phonon
}
HAVE_NULL { 
    DEFINES += NULL_PLAYER
    SOURCES += audioplayerunix.cpp
}
RESOURCES += ../rolisteam.qrc
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
