TEMPLATE = app
CONFIG += qt \
    phonon
CONFIG += HAVE_PHONON
DEFINES += VERSION_MAJOR=2 VERSION_MIDDLE=0 VERSION_MINOR=0
# CONFIG += HAVE_FMOD
# CONFIG += HAVE_NULL

INCLUDEPATH  += /usr/include/poppler/qt4
LIBS         += -L/usr/lib -lpoppler-qt4

QT += core \
    gui \
    network
TARGET = ../bin/rolisteam
include(pdfviewer/pdfviewer.pri)
include(preferences/preferences.pri)
include(map/map.pri)
include(drawitem/drawitem.pri)
include(data/data.pri)
include(charactersheet/charactersheet.pri)
include(userlist/userlist.pri)
include(tchat/tchat.pri)
include(dicepluginmanager/dicepluginmanager.pri)
include(widget/widget.pri)
include(network/network.pri)
include(session/session.pri)
include(minuteseditor/minuteseditor.pri)
DEPENDPATH += . \
    drawitem \
    preferences \
    data \
    userlist \
    charactersheet \
    dicepluginmanager \
    widget \
    session \
    network \
    pdfviewer \
    minuteseditor \
    tchat \
    map
INCLUDEPATH += . \
    drawitem \
    preferences \
    widget \
    session \
    userlist \
    dicepluginmanager \
    data \
    minuteseditor \
    network \
    tchat \
    pdfviewer \
    charactersheet \
    map
HEADERS += displaydisk.h \
    Image.h \
    audioplayer.h \
    MainWindow.h \
    colorselector.h \
    diameterselector.h \
    improvedworkspace.h \
    submdiwindows.h \
    connectionwizzard.h \
    toolbar.h
FORMS += connectionwizzard.ui
SOURCES += displaydisk.cpp \
    Image.cpp \
    main.cpp \
    MainWindow.cpp \
    colorselector.cpp \
    diameterselector.cpp \
    connectionwizzard.cpp \
    improvedworkspace.cpp \
    submdiwindows.cpp \
    toolbar.cpp


DEFINES += PHONON
SOURCES += audioplayer.cpp
QT += phonon

HAVE_NULL { 
    DEFINES += NULL_PLAYER
    SOURCES += audioplayerunix.cpp
}
RESOURCES += ../rolisteam.qrc
documentation.path = bin/doc
unix {
    documentation.path = /usr/share/doc/rolisteam-doc/
}
macx { 
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5
    documentation.path = bin/rolisteam.app/Contents/Resources/doc
}
documentation.files = resources/doc/rolisteam.qhc
target.file= ../bin/rolisteam
target.path=/usr/bin/
INSTALLS += documentation \
    target

UI_DIR = ../ui
MOC_DIR = ../obj
OBJECTS_DIR = ../obj






