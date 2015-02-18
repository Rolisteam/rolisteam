TEMPLATE = app
CONFIG += qt


CONFIG += HAVE_PHONON
#CONFIG += WITH_PDF

DEFINES += VERSION_MAJOR=2 VERSION_MIDDLE=0 VERSION_MINOR=0
# CONFIG += HAVE_FMOD
# CONFIG += HAVE_NULL

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm
PRE_TARGETDEPS += compiler_updateqm_make_all



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
include(chat/chat.pri)
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
    chat \
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
    pdfviewer \
    charactersheet \
    chat \
    map
HEADERS += displaydisk.h \
    Image.h \
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

HAVE_PHONON {
DEFINES += PHONON
HEADERS += audioplayer.h \

SOURCES += audioplayer.cpp \
}

HAVE_NULL { 
    DEFINES += NULL_PLAYER
    SOURCES += audioplayerunix.cpp
}
WITH_PDF {
    DEFINES += WITH_PDF
    include(pdfviewer/pdfviewer.pri)
    #SOURCES +=
    DEPENDPATH += pdfviewer
    INCLUDEPATH  += /usr/include/poppler/qt4
    LIBS         += -L/usr/lib -lpoppler-qt4

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


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport



