cmake_minimum_required(VERSION 3.10)

set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOMOC_MOC_OPTIONS "-nw")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Libs
add_subdirectory(libs)

set(QT_REQUIRED_VERSION "5.15.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets Gui Svg Network Multimedia PrintSupport Qml Quick QuickWidgets QuickControls2 Xml WebEngineWidgets Concurrent)
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG COMPONENTS PdfWidgets)

if(TESTS)
    find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test)
endif()

if(Qt5PdfWidgets_FOUND AND ${WITH_PDF})
    set(HAS_PDF TRUE PARENT_SCOPE)
    message("Enable PDF feature")
    add_definitions(-DWITH_PDF=1)

    set(pdfHeader
        src/controller/view_controller/pdfcontroller.h
        src/pdfviewer/pdfviewer.h)

    set(pdfSource
        src/pdfviewer/pdfviewer.cpp
        src/controller/view_controller/pdfcontroller.cpp)

    set(pdfUiFiles src/pdfviewer/pdfviewer.ui)
endif()


set(HEADERS
    src/controller/gamecontroller.h
    src/controller/networkcontroller.h
    src/controller/playercontroller.h
    src/controller/controllerinterface.h
    src/controller/preferencescontroller.h
    src/controller/instantmessagingcontroller.h
    src/controller/view_controller/imagecontroller.h
    src/controller/view_controller/mediacontrollerbase.h
    src/controller/view_controller/vectorialmapcontroller.h
    src/controller/view_controller/charactersheetcontroller.h
    src/controller/view_controller/webpagecontroller.h
    src/controller/view_controller/sharednotecontroller.h
    src/controller/view_controller/notecontroller.h
    src/controller/view_controller/mindmapcontroller.h
    src/controller/contentcontroller.h
    src/common/controller/theme.h
    src/undoCmd/addlocalcharactercommand.h
    src/undoCmd/removelocalcharactercommand.h
    src/media/mediatype.h
    src/media/charactersheetwindow.h
    src/media/mindmapview.h
    src/audio/audioPlayer.h
    src/audio/musicmodel.h
    src/audio/playerwidget.h
    src/model/profilemodel.h
    src/model/charactermodel.h
    src/model/thememodel.h
    src/model/remoteplayermodel.h
    src/charactersheet/charactersheet.h
    src/charactersheet/charactersheetitem.h
    src/charactersheet/charactersheetmodel.h
    src/charactersheet/csitem.h
    src/charactersheet/field.h
    src/charactersheet/formula/formulamanager.h
    src/charactersheet/formula/nodes/formulanode.h
    src/charactersheet/formula/nodes/operator.h
    src/charactersheet/formula/nodes/parenthesesfnode.h
    src/charactersheet/formula/nodes/scalaroperatorfnode.h
    src/charactersheet/formula/nodes/startnode.h
    src/charactersheet/formula/nodes/valuefnode.h
    src/charactersheet/formula/parsingtoolformula.h
    src/charactersheet/imagemodel.h
    src/charactersheet/propertyhelpers.h
    src/charactersheet/qqmlhelpers.h
    src/charactersheet/rolisteamimageprovider.h
    src/charactersheet/section.h
    src/charactersheet/sheetwidget.h
    src/charactersheet/tablefield.h
    src/qmlchat/instantmessagingmodel.h
    src/qmlchat/messageinterface.h
    src/qmlchat/chatroom.h
    src/qmlchat/errormessage.h
    src/qmlchat/messagemodel.h
    src/qmlchat/textmessage.h
    src/qmlchat/messagefactory.h
    src/qmlchat/dicemessage.h
    src/qmlchat/filteredplayermodel.h
    src/qmlchat/filterinstantmessagingmodel.h
    src/qmlchat/instantmessagingview.h
    src/qmlchat/avatarprovider.h
    src/qmlchat/textwritercontroller.h
    src/qmlchat/chatroomfactory.h
    src/qmlchat/commandmessage.h
    src/qmlchat/chatroomsplittermodel.h
    src/common/controller/logcontroller.h
    src/common/controller/remotelogcontroller.h
    src/common/task/uploadlogtoserver.h
    src/common/widgets/colorbutton.h
    src/common/widgets/logpanel.h
    src/data/character.h
    src/data/characterstate.h
    src/data/charactervision.h
    src/data/cleveruri.h
    src/data/cleverurimimedata.h
    src/data/diceshortcut.h
    src/data/localpersonmodel.h
    src/data/mediacontainer.h
    src/data/person.h
    src/data/player.h
    src/data/resourcesnode.h
    src/data/shortcutmodel.h
    src/data/shortcutvisitor.h
    src/media/image.h
    src/media/mediafactory.h
    src/model/contentmodel.h
    src/model/singlecontenttypemodel.h
    src/model/playerproxymodel.h
    src/model/vmapitemmodel.h
    src/model/imagemodel.h
    src/model/languagemodel.h
    src/network/channel.h
    src/network/channellistpanel.h
    src/network/channelmodel.h
    src/network/clientmanager.h
    src/network/connectionaccepter.h
    src/network/connectionprofile.h
    src/network/connectionretrydialog.h
    src/network/heartbeatsender.h
    src/network/ipbanaccepter.h
    src/network/iprangeaccepter.h
    src/network/messagedispatcher.h
    src/network/networklink.h
    src/network/networkmessage.h
    src/network/networkmessagereader.h
    src/network/networkmessagewriter.h
    src/network/networkreceiver.h
    src/network/passwordaccepter.h
    src/network/receiveevent.h
    src/network/rserver.h
    src/network/selectconnectionprofiledialog.h
    src/network/characterdatamodel.h
    src/network/servermanager.h
    src/network/tcpclient.h
    src/network/timeaccepter.h
    src/network/treeitem.h
    src/noteeditor/format_OO_oasis_/FoColorName.h
    src/noteeditor/format_OO_oasis_/GZipReader.h
    src/noteeditor/format_OO_oasis_/GZipWriter.h
    src/noteeditor/format_OO_oasis_/OOFormat.h
    src/noteeditor/format_OO_oasis_/OOReader.h
    src/noteeditor/format_OO_oasis_/XML_Editor.h
    src/noteeditor/src/notecontainer.h
    src/noteeditor/src/textedit.h
    src/preferences/characterstatemodel.h
    src/preferences/dicealiasmodel.h
    src/preferences/palettemodel.h
    src/preferences/preferencesdialog.h
    src/preferences/preferenceslistener.h
    src/preferences/preferencesmanager.h
    src/preferences/rolisteamtheme.h
    src/services/ipchecker.h
    src/services/tipchecker.h
    src/services/updatechecker.h
    src/services/updaterwindow.h
    src/session/sessionitemmodel.h
    src/session/sessiondock.h
    src/session/sessionview.h
    src/sharededitor/codeeditor.h
    src/sharededitor/document.h
    src/sharededitor/enu.h
    src/sharededitor/finddialog.h
    src/sharededitor/findtoolbar.h
    src/sharededitor/markdownhighlighter.h
    src/sharededitor/participantmodel.h
    src/sharededitor/participantspane.h
    src/sharededitor/sharednote.h
    src/sharededitor/sharednotecontainer.h
    src/sharededitor/utilities.h
    src/undoCmd/openmediacontroller.h
    src/undoCmd/addvmapitem.h
    src/undoCmd/changecoloritem.h
    src/undoCmd/changesizevmapitem.h
    src/undoCmd/removemediacontrollercommand.h
    src/undoCmd/deletevmapitem.h
    src/undoCmd/movevmapitem.h
    src/undoCmd/newmediacontroller.h
    src/undoCmd/addfogofwarchangecommand.h
    src/undoCmd/changestackordervmapcommand.h
    src/undoCmd/showtransparentitemcommand.h
    src/undoCmd/hideotherlayercommand.h
    src/userlist/playermodel.h
    src/userlist/playerspanel.h
    src/userlist/rolisteammimedata.h
    src/userlist/userlistdelegate.h
    src/userlist/userlistview.h
    src/utils/insertionsortedmap.h
    src/vmap/controller/vmapitemfactory.h
    src/vmap/items/anchoritem.h
    src/vmap/items/characteritem.h
    src/vmap/items/childpointitem.h
    src/vmap/items/ellipsitem.h
    src/vmap/items/griditem.h
    src/vmap/items/highlighteritem.h
    src/vmap/items/imageitem.h
    src/vmap/items/lineitem.h
    src/vmap/items/pathitem.h
    src/vmap/items/rectitem.h
    src/vmap/items/ruleitem.h
    src/vmap/items/sightitem.h
    src/vmap/items/textitem.h
    src/vmap/items/visualitem.h
    src/vmap/controller/ellipsecontroller.h
    src/vmap/controller/characteritemcontroller.h
    src/vmap/controller/linecontroller.h
    src/vmap/controller/rectcontroller.h
    src/vmap/controller/textcontroller.h
    src/vmap/controller/gridcontroller.h
    src/vmap/controller/imagecontroller.h
    src/vmap/controller/pathcontroller.h
    src/vmap/controller/sightcontroller.h
    src/vmap/controller/visualitemcontroller.h
    src/vmap/patternmodel.h
    src/vmap/rgraphicsview.h
    src/vmap/vcolorselector.h
    src/vmap/vcolortablechooser.h
    src/vmap/vmap.h
    src/vmap/vmapframe.h
    src/vmap/vmaptoolbar.h
    src/vmap/vmapwizzarddialog.h
    src/vmap/vtoolbar.h
    src/webview/webview.h
    src/widgets/MRichTextEditor/mrichtextedit.h
    src/widgets/MRichTextEditor/mtextedit.h
    src/widgets/aboutrolisteam.h
    src/widgets/centeredcheckbox.h
    src/widgets/circledisplayer.h
    src/widgets/colorselector.h
    src/widgets/delegate.h
    src/widgets/diameterselector.h
    src/widgets/filedirchooser.h
    src/widgets/filepathdelegateitem.h
    src/widgets/flowlayout.h
    src/widgets/notificationzone.h
    src/widgets/gmtoolbox/DiceBookMark/dicebookmarkmodel.h
    src/widgets/gmtoolbox/DiceBookMark/dicebookmarkwidget.h
    src/widgets/gmtoolbox/NameGenerator/namegeneratorwidget.h
    src/widgets/gmtoolbox/NpcMaker/genericmodel.h
    src/widgets/gmtoolbox/NpcMaker/npcmakerwidget.h
    src/widgets/gmtoolbox/UnitConvertor/convertor.h
    src/widgets/gmtoolbox/UnitConvertor/convertoroperator.h
    src/widgets/gmtoolbox/UnitConvertor/customrulemodel.h
    src/widgets/gmtoolbox/UnitConvertor/unit.h
    src/widgets/gmtoolbox/UnitConvertor/unitmodel.h
    src/widgets/gmtoolbox/gamemastertool.h
    src/widgets/workspace.h
    src/widgets/keygeneratordialog.h
    src/widgets/onlinepicturedialog.h
    src/widgets/overlay.h
    src/widgets/persondialog.h
    src/widgets/realslider.h
    src/widgets/shortcuteditordialog.h
    src/widgets/tipofdayviewer.h
    src/worker/messagehelper.h
    src/worker/playermessagehelper.h
    src/worker/vectorialmapmessagehelper.h
    src/worker/modelhelper.h
    src/worker/iohelper.h
    src/worker/networkdownloader.h
    src/worker/convertionhelper.h
    src/worker/autosavecontroller.h
    src/updater/media/vmapupdater.h
    src/updater/media/charactersheetupdater.h
    src/updater/media/mediaupdaterinterface.h
    src/updater/media/mindmapupdater.cpp
    src/updater/media/instantmessagingupdater.h
    src/updater/media/sharednotecontrollerupdater.h
    src/updater/media/webviewupdater.h
    src/updater/vmapitem/rectcontrollerupdater.h
    src/updater/vmapitem/vmapitemcontrollerupdater.h
    src/updater/vmapitem/ellipsecontrollerupdater.h
    src/updater/vmapitem/textcontrollerupdater.h
    src/updater/vmapitem/linecontrollerupdater.h
    src/updater/vmapitem/imagecontrollerupdater.h
    src/updater/vmapitem/pathcontrollerupdater.h
    src/updater/vmapitem/characteritemupdater.h
    libs/diceparser/qmltypesregister.h
    libs/diceparser/diceroller.h
    )


set(SOURCES
    src/controller/gamecontroller.cpp
    src/controller/networkcontroller.cpp
    src/controller/preferencescontroller.cpp
    src/controller/view_controller/mediacontrollerbase.cpp
    src/controller/playercontroller.cpp
    src/controller/contentcontroller.cpp
    src/controller/instantmessagingcontroller.cpp
    src/controller/view_controller/notecontroller.cpp
    src/controller/view_controller/mindmapcontroller.cpp
    src/controller/view_controller/vectorialmapcontroller.cpp
    src/controller/view_controller/sharednotecontroller.cpp
    src/controller/view_controller/webpagecontroller.cpp
    src/controller/view_controller/imagecontroller.cpp
    src/controller/view_controller/charactersheetcontroller.cpp
    src/worker/iohelper.cpp
    src/undoCmd/addlocalcharactercommand.cpp
    src/undoCmd/removelocalcharactercommand.cpp
    src/common/controller/theme.cpp
    src/model/profilemodel.cpp
    src/model/thememodel.cpp
    src/model/vmapitemmodel.cpp
    src/model/charactermodel.cpp
    src/audio/audioPlayer.cpp    
    src/audio/musicmodel.cpp
    src/audio/playerwidget.cpp
    src/qmlchat/textwritercontroller.cpp
    src/qmlchat/errormessage.cpp
    src/qmlchat/chatroom.cpp
    src/qmlchat/messagemodel.cpp
    src/qmlchat/dicemessage.cpp
    src/qmlchat/filteredplayermodel.cpp
    src/qmlchat/messagefactory.cpp
    src/qmlchat/commandmessage.cpp
    src/qmlchat/filterinstantmessagingmodel.cpp
    src/qmlchat/chatroomsplittermodel.cpp
    src/qmlchat/instantmessagingview.cpp
    src/charactersheet/charactersheet.cpp
    src/charactersheet/charactersheetitem.cpp
    src/charactersheet/charactersheetmodel.cpp
    src/media/charactersheetwindow.cpp
    src/charactersheet/csitem.cpp
    src/charactersheet/field.cpp
    src/charactersheet/formula/formulamanager.cpp
    src/charactersheet/formula/nodes/formulanode.cpp
    src/charactersheet/formula/nodes/operator.cpp
    src/charactersheet/formula/nodes/parenthesesfnode.cpp
    src/charactersheet/formula/nodes/scalaroperatorfnode.cpp
    src/charactersheet/formula/nodes/startnode.cpp
    src/charactersheet/formula/nodes/valuefnode.cpp
    src/charactersheet/formula/parsingtoolformula.cpp
    src/charactersheet/imagemodel.cpp
    src/charactersheet/qqmlhelpers.cpp
    src/charactersheet/rolisteamimageprovider.cpp
    src/charactersheet/section.cpp
    src/charactersheet/sheetwidget.cpp
    src/charactersheet/tablefield.cpp
    src/qmlchat/instantmessagingmodel.cpp
    src/qmlchat/messageinterface.cpp
    src/qmlchat/textmessage.cpp
    src/qmlchat/avatarprovider.cpp
    src/qmlchat/chatroomfactory.cpp
    src/common/controller/logcontroller.cpp
    src/common/controller/remotelogcontroller.cpp
    src/common/task/uploadlogtoserver.cpp
    src/common/widgets/colorbutton.cpp
    src/common/widgets/logpanel.cpp
    src/data/character.cpp
    src/data/characterstate.cpp
    src/data/charactervision.cpp
    src/data/cleveruri.cpp
    src/data/cleverurimimedata.cpp
    src/data/diceshortcut.cpp
    src/data/localpersonmodel.cpp
    src/data/mediacontainer.cpp
    src/data/person.cpp
    src/data/player.cpp
    src/data/resourcesnode.cpp
    src/data/shortcutmodel.cpp
    src/data/shortcutvisitor.cpp
    src/media/image.cpp
    src/media/mediafactory.cpp
    src/media/mindmapview.cpp
    src/model/playerproxymodel.cpp
    src/model/remoteplayermodel.cpp
    src/model/singlecontenttypemodel.cpp
    src/model/contentmodel.cpp
    src/model/imagemodel.cpp
    src/model/languagemodel.cpp
    src/network/channel.cpp
    src/network/channellistpanel.cpp
    src/network/channelmodel.cpp
    src/network/clientmanager.cpp
    src/network/connectionaccepter.cpp
    src/network/connectionprofile.cpp
    src/network/characterdatamodel.cpp
    src/network/connectionretrydialog.cpp
    src/network/heartbeatsender.cpp
    src/network/ipbanaccepter.cpp
    src/network/iprangeaccepter.cpp
    src/network/messagedispatcher.cpp
    src/network/networklink.cpp
    src/network/networkmessage.cpp
    src/network/networkmessagereader.cpp
    src/network/networkmessagewriter.cpp
    src/network/passwordaccepter.cpp
    src/network/receiveevent.cpp
    src/network/rserver.cpp
    src/network/selectconnectionprofiledialog.cpp
    src/network/servermanager.cpp
    src/network/tcpclient.cpp
    src/network/timeaccepter.cpp
    src/network/treeitem.cpp
    src/noteeditor/format_OO_oasis_/FoColorName.cpp
    src/noteeditor/format_OO_oasis_/GZip.cpp
    src/noteeditor/format_OO_oasis_/OOFormat.cpp
    src/noteeditor/format_OO_oasis_/OOReader.cpp
    src/noteeditor/format_OO_oasis_/XML_Editor.cpp
    src/noteeditor/src/notecontainer.cpp
    src/noteeditor/src/textedit.cpp
    src/preferences/characterstatemodel.cpp
    src/preferences/dicealiasmodel.cpp
    src/preferences/palettemodel.cpp
    src/preferences/preferencesdialog.cpp
    src/preferences/preferenceslistener.cpp
    src/preferences/preferencesmanager.cpp
    src/preferences/rolisteamtheme.cpp
    src/services/ipchecker.cpp
    src/services/tipchecker.cpp
    src/services/updatechecker.cpp
    src/services/updaterwindow.cpp
    src/session/sessionitemmodel.cpp
    src/session/sessiondock.cpp
    src/session/sessionview.cpp
    src/sharededitor/codeeditor.cpp
    src/sharededitor/document.cpp
    src/sharededitor/finddialog.cpp
    src/sharededitor/findtoolbar.cpp
    src/sharededitor/markdownhighlighter.cpp
    src/sharededitor/participantspane.cpp
    src/sharededitor/participantmodel.cpp
    src/sharededitor/sharednote.cpp
    src/sharededitor/sharednotecontainer.cpp
    src/sharededitor/utilities.cpp
    src/undoCmd/openmediacontroller.cpp
    src/undoCmd/addvmapitem.cpp
    src/undoCmd/changecoloritem.cpp
    src/undoCmd/changesizevmapitem.cpp
    src/undoCmd/removemediacontrollercommand.cpp
    src/undoCmd/newmediacontroller.cpp
    src/undoCmd/deletevmapitem.cpp
    src/undoCmd/movevmapitem.cpp
    src/undoCmd/addfogofwarchangecommand.cpp
    src/undoCmd/changestackordervmapcommand.cpp
    src/undoCmd/showtransparentitemcommand.cpp
    src/undoCmd/hideotherlayercommand.cpp
    src/userlist/playermodel.cpp
    src/userlist/playerspanel.cpp
    src/userlist/rolisteammimedata.cpp
    src/userlist/userlistdelegate.cpp
    src/userlist/userlistview.cpp
    src/vmap/items/anchoritem.cpp
    src/vmap/items/characteritem.cpp
    src/vmap/items/childpointitem.cpp
    src/vmap/items/ellipsitem.cpp
    src/vmap/items/griditem.cpp
    src/vmap/items/highlighteritem.cpp
    src/vmap/items/imageitem.cpp
    src/vmap/items/lineitem.cpp
    src/vmap/items/pathitem.cpp
    src/vmap/items/rectitem.cpp
    src/vmap/items/ruleitem.cpp
    src/vmap/items/sightitem.cpp
    src/vmap/items/textitem.cpp
    src/vmap/items/visualitem.cpp
    src/vmap/controller/vmapitemfactory.cpp
    src/vmap/controller/ellipsecontroller.cpp
    src/vmap/controller/characteritemcontroller.cpp
    src/vmap/controller/linecontroller.cpp
    src/vmap/controller/rectcontroller.cpp
    src/vmap/controller/textcontroller.cpp
    src/vmap/controller/gridcontroller.cpp
    src/vmap/controller/imagecontroller.cpp
    src/vmap/controller/pathcontroller.cpp
    src/vmap/controller/sightcontroller.cpp
    src/vmap/controller/visualitemcontroller.cpp
    src/vmap/patternmodel.cpp
    src/vmap/rgraphicsview.cpp
    src/vmap/vcolorselector.cpp
    src/vmap/vcolortablechooser.cpp
    src/vmap/vmap.cpp
    src/vmap/vmapframe.cpp
    src/vmap/vmaptoolbar.cpp
    src/vmap/vmapwizzarddialog.cpp
    src/vmap/vtoolbar.cpp
    src/webview/webview.cpp
    src/widgets/MRichTextEditor/mrichtextedit.cpp
    src/widgets/MRichTextEditor/mtextedit.cpp
    src/widgets/aboutrolisteam.cpp
    src/widgets/centeredcheckbox.cpp
    src/widgets/circledisplayer.cpp
    src/widgets/colorselector.cpp
    src/widgets/delegate.cpp
    src/widgets/diameterselector.cpp
    src/widgets/filedirchooser.cpp
    src/widgets/filepathdelegateitem.cpp
    src/widgets/notificationzone.cpp
    src/widgets/flowlayout.cpp
    src/widgets/gmtoolbox/DiceBookMark/dicebookmarkmodel.cpp
    src/widgets/gmtoolbox/DiceBookMark/dicebookmarkwidget.cpp
    src/widgets/gmtoolbox/NameGenerator/namegeneratorwidget.cpp
    src/widgets/gmtoolbox/NpcMaker/genericmodel.cpp
    src/widgets/gmtoolbox/NpcMaker/npcmakerwidget.cpp
    src/widgets/gmtoolbox/UnitConvertor/convertor.cpp
    src/widgets/gmtoolbox/UnitConvertor/convertoroperator.cpp
    src/widgets/gmtoolbox/UnitConvertor/customrulemodel.cpp
    src/widgets/gmtoolbox/UnitConvertor/unit.cpp
    src/widgets/gmtoolbox/UnitConvertor/unitmodel.cpp
    src/widgets/gmtoolbox/gamemastertool.cpp
    src/widgets/workspace.cpp
    src/widgets/keygeneratordialog.cpp
    src/widgets/onlinepicturedialog.cpp
    src/widgets/overlay.cpp
    src/widgets/persondialog.cpp
    src/widgets/realslider.cpp
    src/widgets/shortcuteditordialog.cpp
    src/widgets/tipofdayviewer.cpp
    src/updater/media/sharednotecontrollerupdater.cpp
    src/updater/media/vmapupdater.cpp
    src/updater/media/mediaupdaterinterface.cpp
    src/updater/media/mindmapupdater.cpp
    src/updater/media/charactersheetupdater.cpp
    src/updater/media/instantmessagingupdater.cpp
    src/updater/media/webviewupdater.cpp
    src/updater/vmapitem/rectcontrollerupdater.cpp
    src/updater/vmapitem/vmapitemcontrollerupdater.cpp
    src/updater/vmapitem/textcontrollerupdater.cpp
    src/updater/vmapitem/ellipsecontrollerupdater.cpp
    src/updater/vmapitem/linecontrollerupdater.cpp
    src/updater/vmapitem/imagecontrollerupdater.cpp
    src/updater/vmapitem/pathcontrollerupdater.cpp
    src/updater/vmapitem/characteritemupdater.cpp
    src/worker/messagehelper.cpp
    src/worker/networkdownloader.cpp
    src/worker/modelhelper.cpp
    src/worker/vectorialmapmessagehelper.cpp
    src/worker/playermessagehelper.cpp
    src/worker/convertionhelper.cpp
    src/worker/autosavecontroller.cpp
    libs/diceparser/qmltypesregister.cpp
    libs/diceparser/diceroller.cpp
    )


set(UI_FILES
    src/preferences/preferencesdialogbox.ui
    src/audio/audiowidget.ui
    src/sharededitor/findtoolbar.ui
    src/sharededitor/preferencesdialog.ui
    src/sharededitor/participantspane.ui
    src/sharededitor/sharednote.ui
    src/sharededitor/document.ui
    src/sharededitor/announcedocumentdialog.ui
    src/sharededitor/finddialog.ui
    src/sharededitor/firstrundialog.ui
    src/sharededitor/connecttodocument.ui
    src/userlist/playerspanel.ui
    src/common/widgets/logpanel.ui
    src/media/charactersheetwindow.ui
    src/webview/webview.ui
    src/widgets/shortcuteditordialog.ui
    src/widgets/keygeneratordialog.ui
    src/widgets/gmtoolbox/UnitConvertor/convertor.ui
    src/widgets/gmtoolbox/DiceBookMark/dicebookmarkwidget.ui
    src/widgets/gmtoolbox/NpcMaker/npcmakerwidget.ui
    src/widgets/gmtoolbox/NameGenerator/namegeneratorwidget.ui
    src/widgets/tipofdayviewer.ui
    src/widgets/onlinepicturedialog.ui
    src/widgets/MRichTextEditor/mrichtextedit.ui
    src/widgets/aboutrolisteam.ui
    src/widgets/notificationzone.ui
    src/widgets/persondialog.ui
    src/vmap/vmapwizzarddialog.ui
    src/network/selectconnectionprofiledialog.ui
    src/network/channellistpanel.ui
    src/network/connectionretrydialog.ui
    src/session/sessiondock.ui
    src/qmlchat/instantmessagingview.ui
    src/pdfviewer/pdfviewer.ui
    )

set(QRC_SOURCES src/rolisteamqml.qrc src/noteeditor/src/textedit.qrc src/widgets/gmtoolbox/NameGenerator/resources/resources.qrc)


if(Qt5PdfWidgets_FOUND AND ${WITH_PDF})
    message("Link with PDF")
    add_library(core SHARED ${SOURCES} ${HEADERS} ${UI_FILES} ${pdfSource} ${pdfHeader} ${pdfUiFile} ${QRC_SOURCES})
    target_link_libraries(core PUBLIC project_warnings project_options Qt5::Core Qt5::Gui Qt5::Svg Qt5::Widgets Qt5::Network Qt5::Multimedia Qt5::PrintSupport Qt5::Qml Qt5::Quick Qt5::QuickControls2 Qt5::QuickWidgets Qt5::Xml Qt5::WebEngineWidgets Qt5::Concurrent Qt5::PdfWidgets PRIVATE diceparser_shared rmindmap )
else()
    message("Link without PDF")
    add_library(core SHARED ${SOURCES} ${HEADERS} ${UI_FILES} ${QRC_SOURCES})
    target_link_libraries(core PUBLIC project_warnings project_options Qt5::Core Qt5::Gui Qt5::Svg Qt5::Widgets Qt5::Network Qt5::Multimedia Qt5::PrintSupport Qt5::Qml Qt5::Quick Qt5::QuickControls2 Qt5::QuickWidgets Qt5::Xml Qt5::WebEngineWidgets Qt5::Concurrent PRIVATE diceparser_shared rmindmap)
endif()

target_include_directories(core PUBLIC src)
target_include_directories(core PUBLIC libs/mindmap/src)
target_include_directories(core PUBLIC libs/diceparser)
target_include_directories(core PUBLIC libs/diceparser/include)
target_include_directories(core PUBLIC src/charactersheet)
target_include_directories(core PUBLIC src/widgets/MRichTextEditor)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

message(${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_CURRENT_BINARY_DIR})

set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/core_autogen/mocs_compilation.cpp" PROPERTIES COMPILE_FLAGS "-Wno-undefined-reinterpret-cast -Wno-useless-cast")
set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/network_autogen/mocs_compilation.cpp" PROPERTIES COMPILE_FLAGS "-Wno-undefined-reinterpret-cast -Wno-useless-cast")

set(Network_HEADER    
    src/network/channelmodel.h
    src/media/mediatype.h
    src/network/tcpclient.h
    src/network/servermanager.h
    src/network/connectionaccepter.h
    src/network/passwordaccepter.h
    src/network/iprangeaccepter.h
    src/network/ipbanaccepter.h
    src/network/timeaccepter.h
    src/network/channel.h
    src/network/treeitem.h
    src/network/networkmessage.h
    src/network/networkmessagereader.h
    src/network/networkmessagewriter.h
    src/worker/messagehelper.h
    src/network/rserver.h
    src/network/messagedispatcher.h
    src/network/receiveevent.h
    src/data/person.h
    src/data/player.h
    src/data/character.h
    src/data/characterstate.h
    src/data/resourcesnode.h
    src/worker/playermessagehelper.h
    src/common/controller/logcontroller.h)


set(Network_SRC
    src/network/channelmodel.cpp
    src/network/tcpclient.cpp
    src/network/servermanager.cpp
    src/network/connectionaccepter.cpp
    src/network/passwordaccepter.cpp
    src/network/iprangeaccepter.cpp
    src/network/ipbanaccepter.cpp
    src/network/timeaccepter.cpp
    src/network/channel.cpp
    src/network/treeitem.cpp
    src/worker/messagehelper.h
    src/network/networkmessage.cpp
    src/network/networkmessagereader.cpp
    src/network/networkmessagewriter.cpp
    src/network/rserver.cpp
    src/network/messagedispatcher.cpp
    src/network/receiveevent.cpp
    src/data/person.cpp
    src/data/player.cpp
    src/data/character.cpp
    src/data/characterstate.cpp
    src/data/resourcesnode.cpp
    src/worker/playermessagehelper.cpp
    src/common/controller/logcontroller.cpp
)

add_library(network SHARED ${Network_SRC} ${Network_HEADER})
set_target_properties(network PROPERTIES COMPILE_DEFINITIONS "UNIT_TEST=1")
target_link_libraries(network PUBLIC Qt5::Core Qt5::Network Qt5::Gui)

target_include_directories(network PUBLIC src)
target_include_directories(network PUBLIC libs/mindmap/src)
target_include_directories(network PUBLIC libs/diceparser)
target_include_directories(network PUBLIC libs/diceparser/include)
target_include_directories(network PUBLIC src/charactersheet)
target_include_directories(network PUBLIC src/widgets/MRichTextEditor)
target_include_directories(network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS core network
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)
