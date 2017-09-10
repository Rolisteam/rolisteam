##################################################
## Rolisteam Project file
## @author Renaud Guezennec
##################################################

## Generic parameter
CONFIG +=  -console
QT += widgets printsupport multimedia quick qml quickwidgets
#-console
TEMPLATE = app
TARGET = bin/rolisteam
DEPENDPATH += . src
INCLUDEPATH += . src
LANGUAGE = C++
MOC_DIR = bin
OBJECTS_DIR = bin

UI_DIR = bin

isEmpty(PREFIX) {
 PREFIX = /usr/local/bin
}


#DEFINES += DEBUG_MODE
CONFIG += c++11

macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
CONFIG += HAVE_SOUND
#CONFIG += HAVE_NULL

## Translation
TRANSLATIONS =  translations/rolisteam_fr.ts \
                translations/rolisteam.ts \
                translations/rolisteam_de.ts \
                translations/rolisteam_pt_BR.ts \
                translations/rolisteam_ro_RO.ts \
                translations/rolisteam_hu_HU.ts \
                translations/rolisteam_tr.ts \
                translations/rolisteam_es.ts \
                translations/rolisteam_nl_NL.ts \

CODECFORTR = UTF-8

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}
updateTrans.input = rolisteam.pro
updateTrans.output= ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.ts
updateTrans.command= ${QMAKE_LRELEASE} rolisteam.pro

unix:!macx{
# linux only

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateTrans updateqm
PRE_TARGETDEPS += compiler_updateqm_make_all
## End of Translation

##Installation
target.path = $$PREFIX/

}
macx {
target.path = /Applications/
}
INSTALLS += target

include(src/odt/src/src.pri)
include(src/diceparser/diceparser.pri)
include(src/vmap/vmap.pri)
include(src/charactersheet/charactersheet.pri)
include(src/session/session.pri)
include(src/widgets/MRichTextEditor/MRichTextEditor.pri)


#GM TOOL BOX
include(src/widgets/gmtoolbox/NameGenerator/NameGenerator.pri)
include(src/widgets/gmtoolbox/UnitConvertor/UnitConvertor.pri)

   #     src/persondialog.h \
## Source
HEADERS += \
        src/chat/chat.h \
        src/chat/chatlist.h \
        src/chat/chatlistwidget.h \
        src/delegate.h \
        src/Features.h \
        src/Image.h \
        src/localpersonmodel.h \
        src/network/networkmessage.h \
        src/network/networkmessagereader.h \
        src/network/networkmessagewriter.h \
        src/userlist/playersList.h \
        src/userlist/playerslistproxy.h \
        src/userlist/playersListWidget.h \
        src/preferences/preferencesdialog.h \
        src/chat/privatechatdialog.h \
        src/network/receiveevent.h \
        src/chat/chatwindow.h \
        src/services/updatechecker.h \
        src/network/connectionretrydialog.h \
        src/mainwindow.h \
        src/preferences/preferencesmanager.h \
        src/chat/chatbrowser.h \
        src/services/ipchecker.h \
        src/map/mapwizzard.h \
        src/map/newemptymapdialog.h \
        src/widgets/colorbutton.h \
        src/widgets/filedirchooser.h \
        src/audio/audioPlayer.h \
        src/audio/musicmodel.h \
        src/audio/playerwidget.h \
        src/network/networkreceiver.h \
        src/network/networkmanager.h \
        src/improvedworkspace.h \
        src/network/networklink.h \
        src/map/map.h \
        src/widgets/colorselector.h \
        src/widgets/diameterselector.h \
        src/chat/improvedtextedit.h \
        src/widgets/circledisplayer.h \
        src/map/charactertoken.h \
        src/data/mediacontainer.h \
        src/toolsbar.h \
        src/data/cleveruri.h \
        src/data/resourcesnode.h \
        src/map/mapframe.h \
        src/preferences/dicealiasmodel.h \
        src/widgets/onlinepicturedialog.h \
        src/preferences/preferenceslistener.h \
        src/widgets/centeredcheckbox.h \
        src/preferences/palettemodel.h \
        src/preferences/rolisteamtheme.h \
        src/preferences/characterstatemodel.h \
        src/widgets/flowlayout.h \
        src/data/character.h \
        src/data/player.h \
        src/data/person.h \
        src/data/chapter.h \
        src/userlist/rolisteammimedata.h \
        src/userlist/userlistdelegate.h \
        src/userlist/userlistview.h \
        src/widgets/aboutrolisteam.h \
        src/data/charactervision.h \
        src/data/characterstate.h \
        src/widgets/filepathdelegateitem.h \
        src/network/selectconnectionprofiledialog.h \
    src/services/updaterwindow.h \
    src/widgets/realslider.h \
    src/network/heartbeatsender.h \
    src/widgets/persondialog.h


   #     src/persondialog.cpp \

SOURCES += \
        src/chat/chat.cpp \
        src/chat/chatlist.cpp \
        src/chat/chatlistwidget.cpp \
        src/delegate.cpp \
        src/Features.cpp \
        src/Image.cpp \
        src/localpersonmodel.cpp \
        src/main.cpp \
        src/network/networkmessage.cpp \
        src/network/networkmessagereader.cpp \
        src/network/networkmessagewriter.cpp \
        src/userlist/playersList.cpp \
        src/userlist/playerslistproxy.cpp \
        src/userlist/playersListWidget.cpp \
        src/preferences/preferencesdialog.cpp \
        src/chat/privatechatdialog.cpp \
        src/network/receiveevent.cpp \
        src/chat/chatwindow.cpp \
        src/services/updatechecker.cpp \
        src/network/connectionretrydialog.cpp \
        src/mainwindow.cpp \
        src/preferences/preferencesmanager.cpp \
        src/chat/chatbrowser.cpp \
        src/services/ipchecker.cpp \
        src/map/mapwizzard.cpp \
        src/map/newemptymapdialog.cpp \
        src/widgets/colorbutton.cpp \
        src/widgets/filedirchooser.cpp \
        src/audio/audioPlayer.cpp \
        src/audio/musicmodel.cpp \
        src/audio/playerwidget.cpp \
        src/network/networkmanager.cpp \
        src/improvedworkspace.cpp \
        src/network/networklink.cpp \
        src/map/map.cpp \
        src/widgets/colorselector.cpp \
        src/widgets/diameterselector.cpp \
        src/chat/improvedtextedit.cpp \
        src/widgets/circledisplayer.cpp \
        src/data/mediacontainer.cpp \
        src/toolsbar.cpp \
        src/map/charactertoken.cpp \
        src/data/cleveruri.cpp \
        src/map/mapframe.cpp \
        src/preferences/dicealiasmodel.cpp \
        src/widgets/onlinepicturedialog.cpp \
        src/preferences/preferenceslistener.cpp \
        src/widgets/centeredcheckbox.cpp \
        src/preferences/palettemodel.cpp \
        src/preferences/characterstatemodel.cpp \
        src/preferences/rolisteamtheme.cpp \
        src/widgets/flowlayout.cpp \
        src/data/character.cpp \
        src/data/player.cpp \
        src/data/person.cpp \
        src/data/chapter.cpp \
        src/data/resourcesnode.cpp\
        src/userlist/rolisteammimedata.cpp \
        src/userlist/userlistdelegate.cpp \
        src/userlist/userlistview.cpp \
        src/widgets/aboutrolisteam.cpp \
        src/data/charactervision.cpp \
        src/data/characterstate.cpp \
        src/widgets/filepathdelegateitem.cpp \
        src/network/selectconnectionprofiledialog.cpp \
    src/services/updaterwindow.cpp \
    src/widgets/realslider.cpp \
    src/network/heartbeatsender.cpp \
    src/widgets/persondialog.cpp


FORMS += \
    src/network/connectionretrydialog.ui \
    src/map/mapwizzard.ui \
    src/map/newemptymapdialog.ui \
    src/chat/chatwindow.ui \
    src/preferences/preferencesdialogbox.ui \
    src/audio/audiowidget.ui \
    src/mainwindow.ui \
    src/widgets/onlinepicturedialog.ui \
    src/widgets/aboutrolisteam.ui \
    src/network/selectconnectionprofiledialog.ui \
    src/widgets/persondialog.ui
#end source


#Audio configuration
HAVE_SOUND {
 DEFINES+= HAVE_SOUND
}
HAVE_NULL {
 DEFINES+= NULL_PLAYER
}
# End of audio

ICON = resources/logo/rolisteam.icns
RESOURCES += rolisteam.qrc

# Installs
unix:!macx{
QMAKE_CXXFLAGS = "-std=c++11 -fstack-protector -W -Wall -Wextra -pedantic -Wstack-protector -Wno-long-long -Werror"
}

QT += core \
 gui \
 opengl \
 network


# Version
DEFINES += VERSION_MAJOR=1 VERSION_MIDDLE=8 VERSION_MINOR=2
UI_DIR = src

unix{
LIBS += -lz
}
CONFIG += embed_manifest_exe

INCLUDEPATH +=src/audio
OTHER_FILES += \
    src/widgets/widgets.pro \
    $$TRANSLATIONS

#Windows
win32 {
DEFINES  += ZLIB_WINAPI
RC_FILE = "resources/logo/rolisteam.rc"
OTHER_FILES +=resources/logo/rolisteam.rc
LIBS += -L$$PWD/../../lib/zlibapi/dll32/ -lzlibwapi
INCLUDEPATH += $$PWD/../../lib/zlibapi/include
DEPENDPATH += $$PWD/../../lib/zlibapi/include
PRE_TARGETDEPS += $$PWD/../../lib/zlibapi/dll32/zlibwapi.lib
}

DISTFILES += \
    rolisteam.dox
