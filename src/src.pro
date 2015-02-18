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
DEPENDPATH += . \
    drawitem \
    preferences \
    map
INCLUDEPATH += . \
    drawitem \
    preferences \
    map
HEADERS += displaydisk.h \
    ToolBar.h \
#    ClientServeur.h \
    DessinPerso.h \
    EditeurNotes.h \
    Image.h \
    audioplayer.h \
#    Liaison.h \
#    ListeUtilisateurs.h \
    MainWindow.h \
#    Rolisteam_private.h \
    SelecteurCouleur.h \
    SelecteurDiametre.h \
    Tchat.h \
    TextEditAmeliore.h \
    types.h \
    typesApplication.h \
#    variablesGlobales.h \
#   wincompat.h \
    WorkspaceAmeliore.h \
    submdiwindows.h \
    userlistdockwidget.h
FORMS += LecteurAudio.ui \
    userlistdockwidget.ui
SOURCES += displaydisk.cpp \
    ToolBar.cpp \
#    ClientServeur.cpp \
    DessinPerso.cpp \
    EditeurNotes.cpp \
    Image.cpp \
#    Liaison.cpp \
#    ListeUtilisateurs.cpp \
    main.cpp \
    MainWindow.cpp \
    SelecteurCouleur.cpp \
    SelecteurDiametre.cpp \
    Tchat.cpp \
    TextEditAmeliore.cpp \
    WorkspaceAmeliore.cpp \
    submdiwindows.cpp \
    userlistdockwidget.cpp

HAVE_FMOD { 
    DEFINES += FMOD
    SOURCES += audioplayerwin.cpp
#    HEADERS += fmod.h \
#        fmod_errors.h
}
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
VERSION = 1.1
UI_DIR = .
MOC_DIR = ../obj
OBJECTS_DIR = ../obj
