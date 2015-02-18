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
    charactersheet \
    map
HEADERS += displaydisk.h \
    ToolBar.h \
    Image.h \
    audioplayer.h \
    MainWindow.h \
    colorselector.h \
    diameterselector.h \
    improvedworkspace.h \
    submdiwindows.h \
    connectionwizzard.h
FORMS += connectionwizzard.ui
SOURCES += displaydisk.cpp \
    ToolBar.cpp \
    Image.cpp \
    main.cpp \
    MainWindow.cpp \
    colorselector.cpp \
    diameterselector.cpp \
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
DEFINES += VERSION=\\\"2.0.0\\\"
UI_DIR = ../ui
MOC_DIR = ../obj
OBJECTS_DIR = ../obj
