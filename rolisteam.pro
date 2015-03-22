##################################################
##
## Rolisteam project file
##
##################################################

## Generic parameter
CONFIG +=  -console
QT += widgets printsupport multimedia
#-console
TEMPLATE = app
TARGET = bin/rolisteam
DEPENDPATH += . src
INCLUDEPATH += . src
LANGUAGE = C++
MOC_DIR = bin
OBJECTS_DIR = bin
UI_DIR = src

isEmpty(PREFIX) {
 PREFIX = /usr/local
}




macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
CONFIG += HAVE_SOUND
#CONFIG += HAVE_NULL

## Translation
TRANSLATIONS =  traduction/rolisteam_fr.ts \
                traduction/rolisteam_de.ts \
                traduction/rolisteam_it.ts \
                traduction/rolisteam_es.ts \
                traduction/rolisteam_fi.ts \

CODECFORTR = UTF-8

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
#isEmpty(QMAKE_LUPDATE) {
#    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lupdate.exe
#    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lupdate
#}
#updateTrans.input = rolisteam.pro
#updateTrans.output= ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.ts

unix:!macx{
# linux only

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm
PRE_TARGETDEPS += compiler_updateqm_make_all
## End of Translation

##Installation
#binaries.files = bin/rolisteam
TARGET.path = $$PREFIX/
DEPLOYMENT += TARGET
INSTALLS +=TARGET
}

include(src/odt/src/src.pri)

## Source
HEADERS += src/AfficheurDisque.h \
	   src/chat/chat.h \
	   src/chat/chatlist.h \
	   src/chat/chatlistwidget.h \
	   src/network/connectiondialog.h \
           src/delegate.h \
	   src/map/DessinPerso.h \
           src/Features.h \
           src/Image.h \
           src/localpersonmodel.h \
           src/network/networkmessage.h \
           src/network/networkmessagereader.h \
           src/network/networkmessagewriter.h \
           src/persons.h \
           src/persondialog.h \
           src/playersList.h \
           src/playerslistproxy.h \
           src/playersListWidget.h \
           src/preferencesdialog.h \
           src/privatechatdialog.h \
	   src/network/receiveevent.h \
           src/SelecteurCouleur.h \
           src/SelecteurDiametre.h \
	   src/chat/chatwindow.h \
           src/TextEditAmeliore.h \
           src/types.h \
           src/updatechecker.h \
           src/variablesGlobales.h \
    src/network/connectionretrydialog.h \
    src/mainwindow.h \
    src/preferencesmanager.h \
    src/chat/chatbrowser.h \
    src/ipchecker.h \
    src/map/mapwizzard.h \
    src/map/newemptymapdialog.h \
    src/widgets/colorbutton.h \
    src/widgets/filedirchooser.h \
    src/audio/audioPlayer.h \
    src/audio/musicmodel.h \
    src/audio/playerwidget.h \
    src/network/networkreceiver.h \
    src/toolbar.h \
    src/network/networkmanager.h \
    src/improvedworkspace.h \
    src/network/networklink.h \
    src/map/bipmapwindow.h \
    src/map/map.h



SOURCES += src/AfficheurDisque.cpp \
	   src/chat/chat.cpp \
	   src/chat/chatlist.cpp \
	   src/chat/chatlistwidget.cpp \
	   src/network/connectiondialog.cpp \
           src/delegate.cpp \
	   src/map/DessinPerso.cpp \
           src/Features.cpp \
           src/Image.cpp \
           src/initialisation.cpp \
           src/localpersonmodel.cpp \
           src/main.cpp \
           src/network/networkmessage.cpp \
           src/network/networkmessagereader.cpp \
           src/network/networkmessagewriter.cpp \
           src/persons.cpp \
           src/persondialog.cpp \
           src/playersList.cpp \
           src/playerslistproxy.cpp \
           src/playersListWidget.cpp \
           src/preferencesdialog.cpp \
           src/privatechatdialog.cpp \
	   src/network/receiveevent.cpp \
           src/SelecteurCouleur.cpp \
           src/SelecteurDiametre.cpp \
	   src/chat/chatwindow.cpp \
           src/TextEditAmeliore.cpp \
           src/updatechecker.cpp \
    src/network/connectionretrydialog.cpp \
    src/mainwindow.cpp \
    src/preferencesmanager.cpp \
    src/chat/chatbrowser.cpp \
    src/ipchecker.cpp \
    src/map/mapwizzard.cpp \
    src/map/newemptymapdialog.cpp \
    src/widgets/colorbutton.cpp \
    src/widgets/filedirchooser.cpp \
    src/audio/audioPlayer.cpp \
    src/audio/musicmodel.cpp \
    src/audio/playerwidget.cpp \
    src/toolbar.cpp \
    src/network/networkmanager.cpp \
    src/improvedworkspace.cpp \
    src/network/networklink.cpp \
    src/map/bipmapwindow.cpp \
    src/map/map.cpp
#end source


#Audio configuration
HAVE_SOUND {
 DEFINES+= HAVE_SOUND
 HEADERS +=
 SOURCES += 
# QT += phonon
}

HAVE_NULL {
 DEFINES+= NULL_PLAYER
}
# End of audio
ICON = resources/logo/256-icon.icns

RESOURCES += rolisteam.qrc

# Installs
macx{
documentation.path = bin/rolisteam.app/Contents/Resources/doc
}
unix:!macx{
documentation.path = /usr/local/share/rolisteam/
}
documentation.files = resources/doc/*
INSTALLS += documentation

QT += core \
 gui \
 network

# Flags
        #win32:QMAKE_CXXFLAGS = "/Wall /GS /Ob2t"
        #else:QMAKE_CXXFLAGS = "-fstack-protector -W -Wall -Wextra -pedantic -Wstack-protector -Wno-long-long -Werror"


# Version
DEFINES += VERSION_MAJOR=1 VERSION_MIDDLE=7 VERSION_MINOR=0


UI_DIR = src

unix{
LIBS += -lz
}
CONFIG += embed_manifest_exe
FORMS += \
    src/network/connectionretrydialog.ui \
    src/map/mapwizzard.ui \
    src/map/newemptymapdialog.ui \
    src/chat/chatwindow.ui \
    src/preferencesdialogbox.ui \
    src/audio/audiowidget.ui

INCLUDEPATH +=src/audio

OTHER_FILES += \
    src/widgets/widgets.pro \

#Windows
win32:DEFINES  += ZLIB_WINAPI
win32:RC_FILE = "resources/logo/rolisteam.rc"
win32:QMAKE_LFLAGS_RELEASE += /INCREMENTAL:NO
win32:OTHER_FILES +=resources/logo/rolisteam.rc
win32:LIBS += -L$$PWD/../../lib/zlibapi/dll32/ -lzlibwapi
win32:INCLUDEPATH += $$PWD/../../lib/zlibapi/include
win32:DEPENDPATH += $$PWD/../../lib/zlibapi/include
win32:PRE_TARGETDEPS += $$PWD/../../lib/zlibapi/dll32/zlibwapi.lib

