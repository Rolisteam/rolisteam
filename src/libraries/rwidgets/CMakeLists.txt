cmake_minimum_required(VERSION 3.10)

set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOMOC_MOC_OPTIONS "-nw")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(QT_REQUIRED_VERSION "5.15.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets Gui Svg Multimedia Xml Quick QuickWidgets WebEngineWidgets QuickControls2 MultimediaWidgets)
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG COMPONENTS PdfWidgets)

if(Qt5PdfWidgets_FOUND AND ${WITH_PDF})
    set(HAS_PDF TRUE PARENT_SCOPE)
    message("Enable PDF feature")
    add_definitions(-DWITH_PDF=1)

    set(pdfHeader
        mediacontainers/pdfviewer.h)

    set(pdfSource
        mediacontainers/pdfviewer.cpp)

    set(pdfUiFiles mediacontainers/pdfviewer.ui)
else()
    set(pdfHeader)
    set(pdfSource)
    set(pdfUiFiles)
endif()

set(HEADERS
  commands/movevmapitem.h
  customs/centeredcheckbox.h
  customs/circledisplayer.h
  customs/colorlisteditor.h
  customs/colorselector.h
  customs/diameterselector.h
  customs/filedirchooser.h
  customs/imagepatheditor.h
  customs/overlay.h
  customs/playerwidget.h
  customs/realslider.h
  customs/rgraphicsview.h
  customs/shortcutvisitor.h
  customs/updaterwindow.h
  customs/vcolorselector.h
  customs/vcolortablechooser.h
  customs/vmap.h
  customs/workspace.h
  delegates/avatardelegate.h
  delegates/checkboxdelegate.h
  delegates/colordelegate.h
  delegates/delegate.h
  delegates/imagepathdelegateitem.h
  delegates/taglistdelegate.h
  delegates/userlistdelegate.h
  dialogs/aboutrolisteam.h
  dialogs/campaignproperties.h
  dialogs/connectionretrydialog.h
  dialogs/keygeneratordialog.h
  dialogs/imageselectordialog.h
  dialogs/persondialog.h
  dialogs/preferencesdialog.h
  dialogs/selectconnectionprofiledialog.h
  dialogs/shortcuteditordialog.h
  dialogs/tipofdayviewer.h
  dialogs/vmapwizzarddialog.h
  docks/antagonistboard.h
  docks/audioPlayer.h
  docks/campaigndock.h
  docks/channellistpanel.h
  docks/notificationzone.h
  docks/playerspanel.h
  docks/vmaptoolbar.h
  editors/mrichtextedit.h
  editors/mtextedit.h
  editors/noteeditor/format_OO_oasis_/FoColorName.h
  editors/noteeditor/format_OO_oasis_/GZipReader.h
  editors/noteeditor/format_OO_oasis_/GZipWriter.h
  editors/noteeditor/format_OO_oasis_/OOFormat.h
  editors/noteeditor/format_OO_oasis_/OOReader.h
  editors/noteeditor/format_OO_oasis_/XML_Editor.h
  editors/noteeditor/src/notecontainer.h
  editors/noteeditor/src/textedit.h
  editors/sharededitor/codeeditor.h
  editors/sharededitor/document.h
  editors/sharededitor/enu.h
  editors/sharededitor/finddialog.h
  editors/sharededitor/findtoolbar.h
  editors/sharededitor/markdownhighlighter.h
  editors/sharededitor/participantspane.h
  editors/sharededitor/sharednote.h
  editors/sharededitor/sharednotecontainer.h
  editors/sharededitor/utilities.h
  gmtoolbox/DiceBookMark/dicebookmarkmodel.h
  gmtoolbox/DiceBookMark/dicebookmarkwidget.h
  gmtoolbox/NameGenerator/namegeneratorwidget.h
  gmtoolbox/UnitConvertor/convertor.h
  gmtoolbox/UnitConvertor/convertoroperator.h
  gmtoolbox/UnitConvertor/customrulemodel.h
  gmtoolbox/UnitConvertor/unit.h
  gmtoolbox/UnitConvertor/unitmodel.h
  gmtoolbox/gamemastertool.h
  graphicsItems/anchoritem.h
  graphicsItems/characteritem.h
  graphicsItems/childpointitem.h
  graphicsItems/ellipsitem.h
  graphicsItems/griditem.h
  graphicsItems/highlighteritem.h
  graphicsItems/imageitem.h
  graphicsItems/lineitem.h
  graphicsItems/pathitem.h
  graphicsItems/rectitem.h
  graphicsItems/ruleitem.h
  graphicsItems/sightitem.h
  graphicsItems/textitem.h
  graphicsItems/visualitem.h
  layouts/flowlayout.h
  mediacontainers/charactersheetwindow.h
  mediacontainers/image.h
  mediacontainers/instantmessagingview.h
  mediacontainers/mediacontainer.h
  mediacontainers/mindmapview.h
  mediacontainers/vmapframe.h
  mediacontainers/webview.h
  modelviews/campaignview.h
  modelviews/dragabletableview.h
  modelviews/userlistview.h
  toolbars/vtoolbar.h
)


set(SOURCES
  commands/movevmapitem.cpp
  customs/centeredcheckbox.cpp
  customs/circledisplayer.cpp
  customs/colorlisteditor.cpp
  customs/colorselector.cpp
  customs/diameterselector.cpp
  customs/filedirchooser.cpp
  customs/imagepatheditor.cpp
  customs/overlay.cpp
  customs/playerwidget.cpp
  customs/realslider.cpp
  customs/rgraphicsview.cpp
  customs/shortcutvisitor.cpp
  customs/updaterwindow.cpp
  customs/vcolorselector.cpp
  customs/vcolortablechooser.cpp
  customs/vmap.cpp
  customs/workspace.cpp
  delegates/avatardelegate.cpp
  delegates/checkboxdelegate.cpp
  delegates/colordelegate.cpp
  delegates/delegate.cpp
  delegates/imagepathdelegateitem.cpp
  delegates/taglistdelegate.cpp
  delegates/userlistdelegate.cpp
  dialogs/aboutrolisteam.cpp
  dialogs/campaignproperties.cpp
  dialogs/connectionretrydialog.cpp
  dialogs/keygeneratordialog.cpp
  dialogs/imageselectordialog.cpp
  dialogs/persondialog.cpp
  dialogs/preferencesdialog.cpp
  dialogs/selectconnectionprofiledialog.cpp
  dialogs/shortcuteditordialog.cpp
  dialogs/tipofdayviewer.cpp
  dialogs/vmapwizzarddialog.cpp
  docks/antagonistboard.cpp
  docks/audioPlayer.cpp
  docks/campaigndock.cpp
  docks/channellistpanel.cpp
  docks/notificationzone.cpp
  docks/playerspanel.cpp
  docks/vmaptoolbar.cpp
  editors/mrichtextedit.cpp
  editors/mtextedit.cpp
  editors/noteeditor/format_OO_oasis_/FoColorName.cpp
  editors/noteeditor/format_OO_oasis_/GZip.cpp
  editors/noteeditor/format_OO_oasis_/OOFormat.cpp
  editors/noteeditor/format_OO_oasis_/OOReader.cpp
  editors/noteeditor/format_OO_oasis_/XML_Editor.cpp
  editors/noteeditor/src/notecontainer.cpp
  editors/noteeditor/src/textedit.cpp
  editors/sharededitor/codeeditor.cpp
  editors/sharededitor/document.cpp
  editors/sharededitor/finddialog.cpp
  editors/sharededitor/findtoolbar.cpp
  editors/sharededitor/markdownhighlighter.cpp
  editors/sharededitor/participantspane.cpp
  editors/sharededitor/sharednote.cpp
  editors/sharededitor/sharednotecontainer.cpp
  editors/sharededitor/utilities.cpp
  gmtoolbox/DiceBookMark/dicebookmarkmodel.cpp
  gmtoolbox/DiceBookMark/dicebookmarkwidget.cpp
  gmtoolbox/NameGenerator/namegeneratorwidget.cpp
  gmtoolbox/UnitConvertor/convertor.cpp
  gmtoolbox/UnitConvertor/convertoroperator.cpp
  gmtoolbox/UnitConvertor/customrulemodel.cpp
  gmtoolbox/UnitConvertor/unit.cpp
  gmtoolbox/UnitConvertor/unitmodel.cpp
  gmtoolbox/gamemastertool.cpp
  graphicsItems/anchoritem.cpp
  graphicsItems/characteritem.cpp
  graphicsItems/childpointitem.cpp
  graphicsItems/ellipsitem.cpp
  graphicsItems/griditem.cpp
  graphicsItems/highlighteritem.cpp
  graphicsItems/imageitem.cpp
  graphicsItems/lineitem.cpp
  graphicsItems/pathitem.cpp
  graphicsItems/rectitem.cpp
  graphicsItems/ruleitem.cpp
  graphicsItems/sightitem.cpp
  graphicsItems/textitem.cpp
  graphicsItems/visualitem.cpp
  layouts/flowlayout.cpp
  mediacontainers/charactersheetwindow.cpp
  mediacontainers/image.cpp
  mediacontainers/instantmessagingview.cpp
  mediacontainers/mediacontainer.cpp
  mediacontainers/mindmapview.cpp
  mediacontainers/pdfviewer.cpp
  mediacontainers/vmapframe.cpp
  mediacontainers/webview.cpp
  modelviews/campaignview.cpp
  modelviews/dragabletableview.cpp
  modelviews/userlistview.cpp
  toolbars/vtoolbar.cpp
)


set(UI_FILES
  dialogs/campaignproperties.ui
  docks/campaigndock.ui
  docks/antagonistboard.ui
  docks/playerspanel.ui
  dialogs/imageselectordialog.ui
  dialogs/selectconnectionprofiledialog.ui
  editors/mrichtextedit.ui
  dialogs/preferencesdialogbox.ui
)

set(QRC_SOURCES ${CMAKE_SOURCE_DIR}/rolisteam.qrc editors/noteeditor/src/textedit.qrc gmtoolbox/NameGenerator/resources/resources.qrc)

add_library(rwidgets SHARED ${SOURCES} ${HEADERS} ${UI_FILES} ${QRC_SOURCES} ${pdfSource} ${pdfHeader} ${pdfUiFiles})

if(Qt5PdfWidgets_FOUND AND ${WITH_PDF})
    message("Link with PDF")
    target_link_libraries(rwidgets PUBLIC project_warnings project_options Qt5::Core Qt5::Gui Qt5::Svg  Qt5::Widgets Qt5::Xml Qt5::QuickWidgets Qt5::Multimedia Qt5::PdfWidgets Qt5::Quick Qt5::WebEngineWidgets Qt5::QuickControls2 PRIVATE core charactersheet)
else()
    message("Link without PDF")
    target_link_libraries(rwidgets PUBLIC project_warnings project_options Qt5::Core Qt5::Gui Qt5::Svg Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Xml Qt5::Quick Qt5::WebEngineWidgets Qt5::QuickControls2 PRIVATE core charactersheet)
endif()

target_include_directories(rwidgets PUBLIC src)
target_include_directories(rwidgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

message(${CMAKE_CURRENT_BINARY_DIR})

#set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/core_autogen/mocs_compilation.cpp" PROPERTIES COMPILE_FLAGS "-Wno-undefined-reinterpret-cast -Wno-useless-cast")
#set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/network_autogen/mocs_compilation.cpp" PROPERTIES COMPILE_FLAGS "-Wno-undefined-reinterpret-cast -Wno-useless-cast")

install(TARGETS rwidgets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)
