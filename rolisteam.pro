CONFIG += phonon
TEMPLATE = app
TARGET = bin/rolisteam
DEPENDPATH += . src
INCLUDEPATH += . src
LANGUAGE = C++
MOC_DIR = bin
OBJECTS_DIR = bin
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5
#CONFIG += HAVE_PHONON
#CONFIG += HAVE_FMOD
CONFIG += HAVE_NULL
HEADERS += src/AfficheurDisque.h \
           src/BarreOutils.h \
           src/Carte.h \
           src/CarteFenetre.h \
           src/ClientServeur.h \
#           src/constsantesGlobales.h \
           src/DessinPerso.h \
           src/EditeurNotes.h \
           src/Image.h \
           src/Liaison.h \
           src/ListeUtilisateurs.h \
           src/MainWindow.h \
           src/NouveauPlanVide.h \
           src/Rolisteam_private.h \
           src/SelecteurCouleur.h \
           src/SelecteurDiametre.h \
           src/Tchat.h \
           src/TextEditAmeliore.h \
           src/types.h \
           src/typesApplication.h \
           src/variablesGlobales.h \
           src/wincompat.h \
           src/WorkspaceAmeliore.h
SOURCES += src/AfficheurDisque.cpp \
           src/BarreOutils.cpp \
           src/Carte.cpp \
           src/CarteFenetre.cpp \
           src/ClientServeur.cpp \
           src/DessinPerso.cpp \
           src/EditeurNotes.cpp \
           src/Image.cpp \
           src/Liaison.cpp \
           src/ListeUtilisateurs.cpp \
           src/main.cpp \
           src/MainWindow.cpp \
           src/NouveauPlanVide.cpp \
           src/SelecteurCouleur.cpp \
           src/SelecteurDiametre.cpp \
           src/Tchat.cpp \
           src/TextEditAmeliore.cpp \
           src/WorkspaceAmeliore.cpp



HAVE_FMOD {
 DEFINES+= FMOD
 SOURCES +=  src/LecteurAudiowin.cpp
 HEADERS +=  src/fmod.h  src/fmod_errors.h \
           src/LecteurAudio.h
 FORMS += src/LecteurAudio.ui
}

HAVE_PHONON {
 DEFINES+= PHONON
 HEADERS += src/LecteurAudio.h
 FORMS += src/LecteurAudio.ui
 SOURCES +=  src/LecteurAudiounix.cpp
 QT += phonon
}

HAVE_NULL {
 DEFINES+= NULL_PLAYER
}

RESOURCES += rolisteam.qrc
documentation.path = bin/rolisteam.app/Contents/Resources/doc
documentation.files = resources/doc/*
INSTALLS += documentation
QT += core \
 gui \
 network

VERSION = 1.0.2

UI_DIR = src
#QMAKE_CXXFLAGS = "-fstack-protector -W -Wall -Wextra -pedantic -Wstack-protector -Wno-long-long -Wno-overlength-strings -Werror"
