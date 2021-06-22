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
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets Gui Svg Network Multimedia PrintSupport Qml Quick QuickWidgets QuickControls2 Xml WebEngineWidgets Concurrent Sql)
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG COMPONENTS PdfWidgets)

if(TESTS)
    find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test)
endif()

if(Qt5PdfWidgets_FOUND AND ${WITH_PDF})
    set(HAS_PDF TRUE PARENT_SCOPE)
    message("Enable PDF feature")
    add_definitions(-DWITH_PDF=1)
    set(pdfHeader
        controller/view_controller/pdfcontroller.h)

    set(pdfSource
        controller/view_controller/pdfcontroller.cpp)
else()
    set(pdfHeader)
    set(pdfSource)
endif()


set(HEADERS
  controller/antagonistboardcontroller.h
  controller/contentcontroller.h
  controller/controllerinterface.h
  controller/gamecontroller.h
  controller/instantmessagingcontroller.h
  controller/item_controllers/characteritemcontroller.h
  controller/item_controllers/ellipsecontroller.h
  controller/item_controllers/gridcontroller.h
  controller/item_controllers/imagecontroller.h
  controller/item_controllers/linecontroller.h
  controller/item_controllers/pathcontroller.h
  controller/item_controllers/rectcontroller.h
  controller/item_controllers/sightcontroller.h
  controller/item_controllers/textcontroller.h
  controller/item_controllers/visualitemcontroller.h
  controller/item_controllers/vmapitemfactory.h
  #controller/media_controller/charactersheetmediacontroller.h
  #controller/media_controller/imagemediacontroller.h
  #controller/media_controller/mediamanagerbase.h
  #controller/media_controller/notemediacontroller.h
  #controller/media_controller/pdfmediacontroller.h
  #controller/media_controller/sharednotemediacontroller.h
  #controller/media_controller/vectorialmapmediacontroller.h
  #controller/media_controller/webpagemediacontroller.h
  controller/networkcontroller.h
  controller/playercontroller.h
  controller/preferencescontroller.h
  #ontroller/resourcescontroller.h
  controller/view_controller/charactersheetcontroller.h
  controller/view_controller/imagecontroller.h
  controller/view_controller/imageselectorcontroller.h
  controller/view_controller/mediacontrollerbase.h
  controller/view_controller/mindmapcontroller.h
  controller/view_controller/notecontroller.h
  controller/view_controller/sharednotecontroller.h
  controller/view_controller/vectorialmapcontroller.h
  controller/view_controller/webpagecontroller.h
  data/campaign.h
  data/campaigneditor.h
  data/campaignmanager.h
  data/character.h
  data/characterstate.h
  data/charactervision.h
  data/cleveruri.h
  data/cleverurimimedata.h
  data/diceshortcut.h
  data/localpersonmodel.h
  data/media.h
  data/person.h
  data/player.h
  data/resourcesnode.h
  data/rolisteammimedata.h
  data/rolisteamtheme.h
  data/shortcutmodel.h
  media/mediafactory.h
  media/mediatype.h
  model/charactermodel.h
  model/characterstatemodel.h
  model/contentmodel.h
  model/dicealiasmodel.h
  model/genericmodel.h
  model/imagemodel.h
  model/languagemodel.h
  model/mediafilteredmodel.h
  model/mediamodel.h
  model/musicmodel.h
  model/nonplayablecharactermodel.h
  model/palettemodel.h
  model/participantsmodel.h
  model/participantmodel.h
  model/patternmodel.h
  model/playermodel.h
  model/playerproxymodel.h
  model/profilemodel.h
  model/remoteplayermodel.h
  model/singlecontenttypemodel.h
  model/thememodel.h
  model/vmapitemmodel.h
  network/channel.h
  network/channelmodel.h
  network/characterdatamodel.h
  network/clientmanager.h
  network/connectionaccepter.h
  network/connectionprofile.h
  network/heartbeatsender.h
  network/ipbanaccepter.h
  network/iprangeaccepter.h
  network/messagedispatcher.h
  network/networklink.h
  network/networkmessage.h
  network/networkmessagereader.h
  network/networkmessagewriter.h
  network/networkreceiver.h
  network/passwordaccepter.h
  network/receiveevent.h
  network/rserver.h
  network/servermanager.h
  network/tcpclient.h
  network/timeaccepter.h
  network/treeitem.h
  preferences/preferenceslistener.h
  preferences/preferencesmanager.h
  services/ipchecker.h
  services/tipchecker.h
  services/updatechecker.h
  session/sessionitemmodel.h
  #undoCmd/addfieldcommand.h
  undoCmd/addfogofwarchangecommand.h
  undoCmd/addlocalcharactercommand.h
  undoCmd/addnonplayablecharactercommand.h
  undoCmd/addvmapitem.h
  undoCmd/changecoloritem.h
  undoCmd/changesizevmapitem.h
  undoCmd/changestackordervmapcommand.h
  undoCmd/deletevmapitem.h
  undoCmd/hideotherlayercommand.h
  undoCmd/importdatamedia.h
  undoCmd/importimagestate.h
  undoCmd/importmedia.h
  undoCmd/newmediacontroller.h
  undoCmd/openmediacontroller.h
  undoCmd/removelocalcharactercommand.h
  undoCmd/removemediacontrollercommand.h
  undoCmd/showtransparentitemcommand.h
  updater/media/campaignupdater.h
  updater/media/charactersheetupdater.h
  updater/media/instantmessagingupdater.h
  updater/media/mediaupdaterinterface.h
  updater/media/mindmapupdater.h
  updater/media/sharednotecontrollerupdater.h
  updater/media/vmapupdater.h
  updater/media/webviewupdater.h
  updater/vmapitem/characteritemupdater.h
  updater/vmapitem/ellipsecontrollerupdater.h
  updater/vmapitem/imagecontrollerupdater.h
  updater/vmapitem/linecontrollerupdater.h
  updater/vmapitem/pathcontrollerupdater.h
  updater/vmapitem/rectcontrollerupdater.h
  updater/vmapitem/textcontrollerupdater.h
  updater/vmapitem/vmapitemcontrollerupdater.h
  utils/insertionsortedmap.h
  worker/autosavecontroller.h
  worker/convertionhelper.h
  worker/fileserializer.h
  worker/iohelper.h
  worker/messagehelper.h
  worker/modelhelper.h
  worker/networkdownloader.h
  worker/playermessagehelper.h
  worker/utilshelper.h
  worker/vectorialmapmessagehelper.h
)


set(SOURCES
  controller/antagonistboardcontroller.cpp
  controller/contentcontroller.cpp
  controller/gamecontroller.cpp
  controller/instantmessagingcontroller.cpp
  controller/item_controllers/characteritemcontroller.cpp
  controller/item_controllers/ellipsecontroller.cpp
  controller/item_controllers/gridcontroller.cpp
  controller/item_controllers/imagecontroller.cpp
  controller/item_controllers/linecontroller.cpp
  controller/item_controllers/pathcontroller.cpp
  controller/item_controllers/rectcontroller.cpp
  controller/item_controllers/sightcontroller.cpp
  controller/item_controllers/textcontroller.cpp
  controller/item_controllers/visualitemcontroller.cpp
  controller/item_controllers/vmapitemfactory.cpp
  #controller/media_controller/charactersheetmediacontroller.cpp
  #controller/media_controller/imagemediacontroller.cpp
  #controller/media_controller/mediamanagerbase.cpp
  #controller/media_controller/notemediacontroller.cpp
  #controller/media_controller/pdfmediacontroller.cpp
  #controller/media_controller/sharednotemediacontroller.cpp
  #controller/media_controller/vectorialmapmediacontroller.cpp
  #controller/media_controller/webpagemediacontroller.cpp
  controller/networkcontroller.cpp
  controller/playercontroller.cpp
  controller/preferencescontroller.cpp
  #controller/resourcescontroller.cpp
  controller/view_controller/charactersheetcontroller.cpp
  controller/view_controller/imagecontroller.cpp
  controller/view_controller/imageselectorcontroller.cpp
  controller/view_controller/mediacontrollerbase.cpp
  controller/view_controller/mindmapcontroller.cpp
  controller/view_controller/notecontroller.cpp
  controller/view_controller/sharednotecontroller.cpp
  controller/view_controller/vectorialmapcontroller.cpp
  controller/view_controller/webpagecontroller.cpp
  data/campaign.cpp
  data/campaigneditor.cpp
  data/campaignmanager.cpp
  data/character.cpp
  data/characterstate.cpp
  data/charactervision.cpp
  data/cleveruri.cpp
  data/cleverurimimedata.cpp
  data/diceshortcut.cpp
  data/localpersonmodel.cpp
  data/media.cpp
  data/person.cpp
  data/player.cpp
  data/resourcesnode.cpp
  data/rolisteammimedata.cpp
  data/rolisteamtheme.cpp
  data/shortcutmodel.cpp
  media/mediafactory.cpp
  model/charactermodel.cpp
  model/characterstatemodel.cpp
  model/contentmodel.cpp
  model/dicealiasmodel.cpp
  model/imagemodel.cpp
  model/languagemodel.cpp
  model/mediafilteredmodel.cpp
  model/mediamodel.cpp
  model/musicmodel.cpp
  model/nonplayablecharactermodel.cpp
  model/palettemodel.cpp
  model/participantsmodel.cpp
  model/participantmodel.cpp
  model/patternmodel.cpp
  model/playermodel.cpp
  model/playerproxymodel.cpp
  model/profilemodel.cpp
  model/genericmodel.cpp
  model/remoteplayermodel.cpp
  model/singlecontenttypemodel.cpp
  model/thememodel.cpp
  model/vmapitemmodel.cpp
  network/channel.cpp
  network/channelmodel.cpp
  network/characterdatamodel.cpp
  network/clientmanager.cpp
  network/connectionaccepter.cpp
  network/connectionprofile.cpp
  network/heartbeatsender.cpp
  network/ipbanaccepter.cpp
  network/iprangeaccepter.cpp
  network/messagedispatcher.cpp
  network/networklink.cpp
  network/networkmessage.cpp
  network/networkmessagereader.cpp
  network/networkmessagewriter.cpp
  network/passwordaccepter.cpp
  network/receiveevent.cpp
  network/rserver.cpp
  network/servermanager.cpp
  network/tcpclient.cpp
  network/timeaccepter.cpp
  network/treeitem.cpp
  preferences/preferenceslistener.cpp
  preferences/preferencesmanager.cpp
  services/ipchecker.cpp
  services/tipchecker.cpp
  services/updatechecker.cpp
  session/sessionitemmodel.cpp
  #undoCmd/addfieldcommand.cpp
  undoCmd/addfogofwarchangecommand.cpp
  undoCmd/addlocalcharactercommand.cpp
  undoCmd/addnonplayablecharactercommand.cpp
  undoCmd/addvmapitem.cpp
  undoCmd/changecoloritem.cpp
  undoCmd/changesizevmapitem.cpp
  undoCmd/changestackordervmapcommand.cpp
  undoCmd/deletevmapitem.cpp
  undoCmd/hideotherlayercommand.cpp
  undoCmd/importdatamedia.cpp
  undoCmd/importimagestate.cpp
  undoCmd/importmedia.cpp
  undoCmd/newmediacontroller.cpp
  undoCmd/openmediacontroller.cpp
  undoCmd/removelocalcharactercommand.cpp
  undoCmd/removemediacontrollercommand.cpp
  undoCmd/showtransparentitemcommand.cpp
  updater/media/campaignupdater.cpp
  updater/media/charactersheetupdater.cpp
  updater/media/instantmessagingupdater.cpp
  updater/media/mediaupdaterinterface.cpp
  updater/media/mindmapupdater.cpp
  updater/media/sharednotecontrollerupdater.cpp
  updater/media/vmapupdater.cpp
  updater/media/webviewupdater.cpp
  updater/vmapitem/characteritemupdater.cpp
  updater/vmapitem/ellipsecontrollerupdater.cpp
  updater/vmapitem/imagecontrollerupdater.cpp
  updater/vmapitem/linecontrollerupdater.cpp
  updater/vmapitem/pathcontrollerupdater.cpp
  updater/vmapitem/rectcontrollerupdater.cpp
  updater/vmapitem/textcontrollerupdater.cpp
  updater/vmapitem/vmapitemcontrollerupdater.cpp
  worker/autosavecontroller.cpp
  worker/convertionhelper.cpp
  worker/fileserializer.cpp
  worker/iohelper.cpp
  worker/messagehelper.cpp
  worker/modelhelper.cpp
  worker/networkdownloader.cpp
  worker/playermessagehelper.cpp
  worker/utilshelper.cpp
  worker/vectorialmapmessagehelper.cpp
)


# Qt5::PdfWidgets Qt5::Gui Qt5::Svg Qt5::Multimedia  Qt5::PrintSupport Qt5::Qml Qt5::Quick Qt5::QuickControls2 Qt5::QuickWidgets Qt5::Xml Qt5::WebEngineWidgets
if(Qt5PdfWidgets_FOUND AND ${WITH_PDF})
    message("Link with PDF")
    add_library(core SHARED ${SOURCES} ${HEADERS} ${pdfSource} ${pdfHeader} ${QRC_SOURCES})
    target_link_libraries(core PUBLIC project_warnings project_options Qt5::Core  Qt5::Network Qt5::Multimedia Qt5::Concurrent PRIVATE diceparser_shared rmindmap common_core charactersheet_widgets)
endif()

target_include_directories(core PUBLIC ../)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

message(${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_CURRENT_BINARY_DIR})

set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/core_autogen/mocs_compilation.cpp" PROPERTIES COMPILE_FLAGS "-Wno-undefined-reinterpret-cast -Wno-useless-cast")
set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/network_autogen/mocs_compilation.cpp" PROPERTIES COMPILE_FLAGS "-Wno-undefined-reinterpret-cast -Wno-useless-cast")

set(Network_HEADER    
    network/channelmodel.h
    media/mediatype.h
    network/tcpclient.h
    network/servermanager.h
    network/connectionaccepter.h
    network/passwordaccepter.h
    network/iprangeaccepter.h
    network/ipbanaccepter.h
    network/timeaccepter.h
    network/channel.h
    network/treeitem.h
    network/networkmessage.h
    network/networkmessagereader.h
    network/networkmessagewriter.h
    worker/messagehelper.h
    network/rserver.h
    network/messagedispatcher.h
    network/receiveevent.h
    data/person.h
    data/player.h
    data/character.h
    data/characterstate.h
    data/resourcesnode.h
    worker/playermessagehelper.h
)


set(Network_SRC
    network/channelmodel.cpp
    network/tcpclient.cpp
    network/servermanager.cpp
    network/connectionaccepter.cpp
    network/passwordaccepter.cpp
    network/iprangeaccepter.cpp
    network/ipbanaccepter.cpp
    network/timeaccepter.cpp
    network/channel.cpp
    network/treeitem.cpp
    worker/messagehelper.h
    network/networkmessage.cpp
    network/networkmessagereader.cpp
    network/networkmessagewriter.cpp
    network/rserver.cpp
    network/messagedispatcher.cpp
    network/receiveevent.cpp
    data/person.cpp
    data/player.cpp
    data/character.cpp
    data/characterstate.cpp
    data/resourcesnode.cpp
    worker/playermessagehelper.cpp
)

add_library(network SHARED ${Network_SRC} ${Network_HEADER})
set_target_properties(network PROPERTIES COMPILE_DEFINITIONS "UNIT_TEST=1")
target_link_libraries(network PUBLIC Qt5::Core Qt5::Network Qt5::Gui)

target_include_directories(network PUBLIC ../)
target_include_directories(network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS core network
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)
