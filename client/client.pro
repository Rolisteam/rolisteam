TEMPLATE = app
TARGET = bin/rolisteam
LANGUAGE = C++

DEPENDPATH += .
INCLUDEPATH += .

isEmpty(PREFIX) {
 PREFIX = /usr/local/bin
}
CONFIG += c++11


macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
CONFIG += HAVE_SOUND

QT += core gui opengl network widgets printsupport multimedia quick qml quickwidgets webenginewidgets webview

## Translation
TRANSLATIONS =  ../translations/rolisteam_fr.ts \
                ../translations/rolisteam.ts \
                ../translations/rolisteam_de.ts \
                ../translations/rolisteam_pt_BR.ts \
                ../translations/rolisteam_hu_HU.ts \
                ../translations/rolisteam_tr.ts \
                ../translations/rolisteam_es.ts \

CODECFORTR = UTF-8

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}
updateTrans.input = src.pro
updateTrans.output= ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.ts
updateTrans.command= ${QMAKE_LRELEASE} src.pro


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

include(noteeditor/noteeditor.pri)
include(diceparser/diceparser.pri)
include(vmap/vmap.pri)
include(charactersheet/charactersheet.pri)
include(session/session.pri)
include(widgets/MRichTextEditor/MRichTextEditor.pri)
include(sharededitor/sharededitor.pri)
include(undoCmd/undoCmd.pri)



#GM TOOL BOX
include(widgets/gmtoolbox/NameGenerator/NameGenerator.pri)
include(widgets/gmtoolbox/UnitConvertor/UnitConvertor.pri)
include(widgets/gmtoolbox/DiceBookMark/DiceBookMark.pri)

#Audio configuration
HAVE_SOUND {
 DEFINES+= HAVE_SOUND
}
HAVE_nullptr {
 DEFINES+= nullptr_PLAYER
}
# End of audio

## Source
HEADERS += \
        chat/chat.h \
        chat/chatlist.h \
        chat/chatlistwidget.h \
        delegate.h \
        Features.h \
        Image.h \
        localpersonmodel.h \
        network/networkmessage.h \
        network/networkmessagereader.h \
        network/networkmessagewriter.h \
        userlist/playersList.h \
        userlist/playerslistproxy.h \
        userlist/playersListWidget.h \
        preferences/preferencesdialog.h \
        chat/privatechatdialog.h \
        network/receiveevent.h \
        chat/chatwindow.h \
        services/updatechecker.h \
        network/connectionretrydialog.h \
        mainwindow.h \
        preferences/preferencesmanager.h \
        chat/chatbrowser.h \
        services/ipchecker.h \
        map/mapwizzard.h \
        map/newemptymapdialog.h \
        widgets/colorbutton.h \
        widgets/filedirchooser.h \
        audio/audioPlayer.h \
        audio/musicmodel.h \
        audio/playerwidget.h \
        network/networkreceiver.h \
        network/networkmanager.h \
        improvedworkspace.h \
        network/networklink.h \
        map/map.h \
        widgets/colorselector.h \
        widgets/diameterselector.h \
        chat/improvedtextedit.h \
        widgets/circledisplayer.h \
        map/charactertoken.h \
        data/mediacontainer.h \
        toolsbar.h \
        data/cleveruri.h \
        data/resourcesnode.h \
        map/mapframe.h \
        preferences/dicealiasmodel.h \
        widgets/onlinepicturedialog.h \
        preferences/preferenceslistener.h \
        widgets/centeredcheckbox.h \
        preferences/palettemodel.h \
        preferences/rolisteamtheme.h \
        preferences/characterstatemodel.h \
        widgets/flowlayout.h \
        data/character.h \
        data/player.h \
        data/person.h \
        data/chapter.h \
        userlist/rolisteammimedata.h \
        userlist/userlistdelegate.h \
        userlist/userlistview.h \
        widgets/aboutrolisteam.h \
        data/charactervision.h \
        data/characterstate.h \
        widgets/filepathdelegateitem.h \
        network/selectconnectionprofiledialog.h \
    services/updaterwindow.h \
    widgets/realslider.h \
    network/heartbeatsender.h \
    widgets/persondialog.h \
    network/connectionprofile.h \
    network/tcpclient.h \
    network/channelmodel.h \
    network/messagedispatcher.h \
    network/channel.h \
    network/treeitem.h \
    network/channellistpanel.h \
    network/servermanager.h \
    network/ipbanaccepter.h \
    network/iprangeaccepter.h \
    network/passwordaccepter.h \
    network/timeaccepter.h \
    network/connectionaccepter.h \
    network/rserver.h \
    services/tipchecker.h \
    widgets/tipofdayviewer.h \
    data/shortcutmodel.h \
    data/shortcutvisitor.h \
    widgets/shortcuteditordialog.h \
    widgets/gmtoolbox/gamemastertool.h


   #     persondialog.cpp \

SOURCES += \
        chat/chat.cpp \
        chat/chatlist.cpp \
        chat/chatlistwidget.cpp \
        delegate.cpp \
        Features.cpp \
        Image.cpp \
        localpersonmodel.cpp \
        main.cpp \
        network/networkmessage.cpp \
        network/networkmessagereader.cpp \
        network/networkmessagewriter.cpp \
        userlist/playersList.cpp \
        userlist/playerslistproxy.cpp \
        userlist/playersListWidget.cpp \
        preferences/preferencesdialog.cpp \
        chat/privatechatdialog.cpp \
        network/receiveevent.cpp \
        chat/chatwindow.cpp \
        services/updatechecker.cpp \
        network/connectionretrydialog.cpp \
        mainwindow.cpp \
        preferences/preferencesmanager.cpp \
        chat/chatbrowser.cpp \
        services/ipchecker.cpp \
        map/mapwizzard.cpp \
        map/newemptymapdialog.cpp \
        widgets/colorbutton.cpp \
        widgets/filedirchooser.cpp \
        audio/audioPlayer.cpp \
        audio/musicmodel.cpp \
        audio/playerwidget.cpp \
        network/networkmanager.cpp \
        improvedworkspace.cpp \
        network/networklink.cpp \
        map/map.cpp \
        widgets/colorselector.cpp \
        widgets/diameterselector.cpp \
        chat/improvedtextedit.cpp \
        widgets/circledisplayer.cpp \
        data/mediacontainer.cpp \
        toolsbar.cpp \
        map/charactertoken.cpp \
        data/cleveruri.cpp \
        map/mapframe.cpp \
        preferences/dicealiasmodel.cpp \
        widgets/onlinepicturedialog.cpp \
        preferences/preferenceslistener.cpp \
        widgets/centeredcheckbox.cpp \
        preferences/palettemodel.cpp \
        preferences/characterstatemodel.cpp \
        preferences/rolisteamtheme.cpp \
        widgets/flowlayout.cpp \
        data/character.cpp \
        data/player.cpp \
        data/person.cpp \
        data/chapter.cpp \
        data/resourcesnode.cpp\
        userlist/rolisteammimedata.cpp \
        userlist/userlistdelegate.cpp \
        userlist/userlistview.cpp \
        widgets/aboutrolisteam.cpp \
        data/charactervision.cpp \
        data/characterstate.cpp \
        widgets/filepathdelegateitem.cpp \
        network/selectconnectionprofiledialog.cpp \
    services/updaterwindow.cpp \
    widgets/realslider.cpp \
    network/heartbeatsender.cpp \
    widgets/persondialog.cpp \
    network/connectionprofile.cpp \
    network/tcpclient.cpp \
    network/channelmodel.cpp \
    network/messagedispatcher.cpp \
    network/channel.cpp \
    network/treeitem.cpp \
    network/channellistpanel.cpp \
    network/servermanager.cpp \
    network/ipbanaccepter.cpp \
    network/iprangeaccepter.cpp \
    network/passwordaccepter.cpp \
    network/timeaccepter.cpp \
    network/connectionaccepter.cpp \
    network/rserver.cpp \
    services/tipchecker.cpp \
    widgets/tipofdayviewer.cpp \
    data/shortcutmodel.cpp \
    data/shortcutvisitor.cpp \
    widgets/shortcuteditordialog.cpp \
    widgets/gmtoolbox/gamemastertool.cpp


FORMS += \
    network/connectionretrydialog.ui \
    map/mapwizzard.ui \
    map/newemptymapdialog.ui \
    preferences/preferencesdialogbox.ui \
    audio/audiowidget.ui \
    mainwindow.ui \
    widgets/onlinepicturedialog.ui \
    widgets/aboutrolisteam.ui \
    network/selectconnectionprofiledialog.ui \
    widgets/persondialog.ui \
    network/channellistpanel.ui \
    widgets/tipofdayviewer.ui \
    widgets/shortcuteditordialog.ui


# Installs
unix:!macx{
QMAKE_CXXFLAGS = "-std=c++11 -fstack-protector -W -Wall -Wextra -pedantic -Wstack-protector -Wno-long-long -Werror -Wall -Wextra -Wnon-virtual-dtor"
}




ICON = ../resources/logo/rolisteam.icns

# Version
DEFINES += VERSION_MAJOR=1 VERSION_MIDDLE=9 VERSION_MINOR=0

RESOURCES += $$PWD/../rolisteam.qrc

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
RC_FILE = "../resources/logo/rolisteam.rc"
OTHER_FILES +=../resources/logo/rolisteam.rc
LIBS += -L$$PWD/../../../lib/zlibapi/dll32/ -lzlibwapi
INCLUDEPATH += $$PWD/../../../lib/zlibapi/include
DEPENDPATH += $$PWD/../../../lib/zlibapi/include
PRE_TARGETDEPS += $$PWD/../../../lib/zlibapi/dll32/zlibwapi.lib
}

DISTFILES += \
    rolisteam.dox \
    undoCmd/undoCmd.pri

