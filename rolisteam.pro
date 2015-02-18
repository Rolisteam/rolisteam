CONFIG += phonon debug console
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
           src/chatlist.h \
           src/chatlistwidget.h \
           src/ClientServeur.h \
           src/colorbutton.h \
           src/connectiondialog.h \
           src/datareader.h \
           src/datawriter.h \
           src/delegate.h \
           src/DessinPerso.h \
           src/EditeurNotes.h \
           src/Features.h \
           src/Image.h \
           src/initialisation.h \
           src/Liaison.h \
           src/MainWindow.h \
           src/NouveauPlanVide.h \
           src/persons.h \
           src/persondialog.h \
           src/playersList.h \
           src/playersListWidget.h \
           src/preferencesdialog.h \
           src/receiveevent.h \
           src/Rolisteam_private.h \
           src/SelecteurCouleur.h \
           src/SelecteurDiametre.h \
           src/Tchat.h \
           src/TextEditAmeliore.h \
           src/types.h \
           src/typesApplication.h \
           src/updatechecker.h \
           src/variablesGlobales.h \
           src/wincompat.h \
           src/WorkspaceAmeliore.h
SOURCES += src/AfficheurDisque.cpp \
           src/BarreOutils.cpp \
           src/Carte.cpp \
           src/CarteFenetre.cpp \
           src/chatlist.cpp \
           src/chatlistwidget.cpp \
           src/ClientServeur.cpp \
           src/colorbutton.cpp \
           src/connectiondialog.cpp \
           src/datareader.cpp \
           src/datawriter.cpp \
           src/delegate.cpp \
           src/DessinPerso.cpp \
           src/EditeurNotes.cpp \
           src/Features.cpp \
           src/Image.cpp \
           src/initialisation.cpp \
           src/Liaison.cpp \
           src/main.cpp \
           src/MainWindow.cpp \
           src/NouveauPlanVide.cpp \
           src/persons.cpp \
           src/persondialog.cpp \
           src/playersList.cpp \
           src/playersListWidget.cpp \
           src/preferencesdialog.cpp \
           src/receiveevent.cpp \
           src/SelecteurCouleur.cpp \
           src/SelecteurDiametre.cpp \
           src/Tchat.cpp \
           src/TextEditAmeliore.cpp \
           src/updatechecker.cpp \
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
 SOURCES +=  src/LecteurAudio.cpp
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
