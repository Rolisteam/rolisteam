CONFIG += phonon console
TEMPLATE = app
TARGET = bin/rolisteam
DEPENDPATH += . src
INCLUDEPATH += . src
LANGUAGE = C++
MOC_DIR = bin
OBJECTS_DIR = bin
#DEFINES += VERSION=\\\"1.0.2\\\"
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5
CONFIG += HAVE_PHONON
#CONFIG += HAVE_NULL
TRANSLATIONS = traduction/rolisteam_fr.ts
CODECFORTR = UTF-8
HEADERS += src/AfficheurDisque.h \
           src/BarreOutils.h \
           src/Carte.h \
           src/CarteFenetre.h \
           src/chat.h \
           src/chatlist.h \
           src/chatlistwidget.h \
           src/ClientServeur.h \
           src/colorbutton.h \
           src/connectiondialog.h \
           src/delegate.h \
           src/DessinPerso.h \
           src/EditeurNotes.h \
           src/Features.h \
           src/Image.h \
           src/initialisation.h \
           src/Liaison.h \
           src/localpersonmodel.h \
           src/networkmessage.h \
           src/networkmessagereader.h \
           src/networkmessagewriter.h \
           src/NouveauPlanVide.h \
           src/persons.h \
           src/persondialog.h \
           src/playersList.h \
           src/playerslistproxy.h \
           src/playersListWidget.h \
           src/preferencesdialog.h \
           src/privatechatdialog.h \
           src/receiveevent.h \
           src/SelecteurCouleur.h \
           src/SelecteurDiametre.h \
           src/chatwindow.h \
           src/TextEditAmeliore.h \
           src/types.h \
           src/typesApplication.h \
           src/updatechecker.h \
           src/variablesGlobales.h \
           src/WorkspaceAmeliore.h \
    src/connectionretrydialog.h \
    src/mainwindow.h

SOURCES += src/AfficheurDisque.cpp \
           src/BarreOutils.cpp \
           src/Carte.cpp \
           src/CarteFenetre.cpp \
           src/chat.cpp \
           src/chatlist.cpp \
           src/chatlistwidget.cpp \
           src/ClientServeur.cpp \
           src/colorbutton.cpp \
           src/connectiondialog.cpp \
           src/delegate.cpp \
           src/DessinPerso.cpp \
           src/EditeurNotes.cpp \
           src/Features.cpp \
           src/Image.cpp \
           src/initialisation.cpp \
           src/Liaison.cpp \
           src/localpersonmodel.cpp \
           src/main.cpp \
           src/NouveauPlanVide.cpp \
           src/networkmessage.cpp \
           src/networkmessagereader.cpp \
           src/networkmessagewriter.cpp \
           src/persons.cpp \
           src/persondialog.cpp \
           src/playersList.cpp \
           src/playerslistproxy.cpp \
           src/playersListWidget.cpp \
           src/preferencesdialog.cpp \
           src/privatechatdialog.cpp \
           src/receiveevent.cpp \
           src/SelecteurCouleur.cpp \
           src/SelecteurDiametre.cpp \
           src/chatwindow.cpp \
           src/TextEditAmeliore.cpp \
           src/updatechecker.cpp \
           src/WorkspaceAmeliore.cpp \
    src/connectionretrydialog.cpp \
    src/mainwindow.cpp

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

#VERSION = 1.0.3
DEFINES += VERSION_MAJOR=1 VERSION_MIDDLE=5 VERSION_MINOR=2

#debug {
#DEFINES += QT_NO_WARNING_OUTPUT
#}
#release {
#DEFINES += QT_FATAL_WARNINGS
#}


UI_DIR = src
QMAKE_CXXFLAGS = "-fstack-protector -W -Wall -Wextra -pedantic -Wstack-protector -Wno-long-long -Wno-overlength-strings -Werror"

FORMS += \
    src/timerdialog.ui \
    src/connectionretrydialog.ui
