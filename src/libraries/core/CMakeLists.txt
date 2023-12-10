cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOMOC_MOC_OPTIONS "-nw")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
if(MSVC)
  set(CMAKE_CXX_FLAGS "-EHsc")
else()
  set(CMAKE_CXX_FLAGS "-Wall")
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Widgets Gui Svg Network Multimedia PrintSupport Qml Quick Xml QuickWidgets QuickControls2 Xml WebEngineWidgets WebEngineCore Concurrent Sql PdfWidgets StateMachine REQUIRED)

if(TESTS)
    find_package(Qt6 COMPONENTS Test REQUIRED)
endif()

#===============================================================
set(inc include)
set(Network_HEADER
    ${inc}/network_global.h
    ${inc}/network/serverconnectionmanager.h
    ${inc}/network/serverconnection.h
    ${inc}/network/channelmodel.h
    ${inc}/media/mediatype.h
    ${inc}/network/connectionaccepter.h
    ${inc}/network/connectionprofile.h
    ${inc}/network/passwordaccepter.h
    ${inc}/network/iprangeaccepter.h
    ${inc}/network/ipbanaccepter.h
    ${inc}/network/characterdatamodel.h
    ${inc}/network/clientmanager.h
    ${inc}/network/clientconnection.h
    ${inc}/network/timeaccepter.h
    ${inc}/network/channel.h
    ${inc}/network/treeitem.h
    ${inc}/network/networkmessage.h
    ${inc}/network/networkmessagereader.h
    ${inc}/network/networkmessagewriter.h
    ${inc}/network/rserver.h
    ${inc}/network/messagedispatcher.h
    ${inc}/network/receiveevent.h
    ${inc}/worker/networkhelper.h
    ${inc}/data/person.h
    ${inc}/data/player.h
    ${inc}/data/character.h
    ${inc}/data/characterstate.h
    ${inc}/data/resourcesnode.h
    ${inc}/worker/playermessagehelper.h
    ${inc}/updater/controller/servermanagerupdater.h
    ${inc}/network/upnp/upnpnat.h
     ${inc}/data/charactervision.h
)

set(Network_SRC
    src/network/channelmodel.cpp
    src/network/serverconnectionmanager.cpp
    src/network/serverconnection.cpp
    src/network/connectionprofile.cpp
    src/network/clientmanager.cpp
    src/network/clientconnection.cpp
    src/network/connectionaccepter.cpp
    src/network/passwordaccepter.cpp
    src/network/iprangeaccepter.cpp
    src/network/ipbanaccepter.cpp
    src/network/timeaccepter.cpp
    src/network/channel.cpp
    src/network/treeitem.cpp
    src/network/characterdatamodel.cpp
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
    src/worker/networkhelper.cpp
    src/worker/playermessagehelper.cpp
    src/updater/controller/servermanagerupdater.cpp
    src/network/upnp/upnpnat.cpp
    src/data/charactervision.cpp
)
message(${Network_SRC} ${Network_HEADER})
add_library(network SHARED ${Network_SRC} ${Network_HEADER})
set_target_properties(network PROPERTIES PUBLIC_HEADER "${Network_HEADER}")
set_target_properties(network PROPERTIES COMPILE_DEFINITIONS "UNIT_TEST=1")
target_compile_definitions(network PRIVATE NETWORK_LIBRARY)
target_link_libraries(network
    PUBLIC
        Qt6::Core
        Qt6::Network
        Qt6::Gui
        Qt6::StateMachine
        Qt6::Xml
    PRIVATE
        common_core
        utils
        charactersheet
 )
target_include_directories(network
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/upnp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/upnp>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)





#===============================================================


set(HEADERS
  ${inc}/core_global.h
  ${inc}/controller/antagonistboardcontroller.h
  ${inc}/controller/view_controller/pdfcontroller.h
  ${inc}/controller/contentcontroller.h
  ${inc}/controller/controllerinterface.h
  ${inc}/controller/gamecontroller.h
  ${inc}/controller/instantmessagingcontroller.h
  ${inc}/controller/item_controllers/characteritemcontroller.h
  ${inc}/controller/item_controllers/ellipsecontroller.h
  ${inc}/controller/item_controllers/gridcontroller.h
  ${inc}/controller/item_controllers/imageitemcontroller.h
  ${inc}/controller/item_controllers/linecontroller.h
  ${inc}/controller/item_controllers/pathcontroller.h
  ${inc}/controller/item_controllers/rectcontroller.h
  ${inc}/controller/item_controllers/sightcontroller.h
  ${inc}/controller/item_controllers/textcontroller.h
  ${inc}/controller/item_controllers/visualitemcontroller.h
  ${inc}/controller/item_controllers/vmapitemfactory.h
  ${inc}/controller/networkcontroller.h
  ${inc}/controller/audioplayercontroller.h
  ${inc}/controller/audiocontroller.h
  ${inc}/controller/playercontroller.h
  ${inc}/controller/preferencescontroller.h
  ${inc}/controller/view_controller/charactersheetcontroller.h
  ${inc}/controller/view_controller/imagecontroller.h
  ${inc}/controller/view_controller/imageselectorcontroller.h
  ${inc}/controller/view_controller/mediacontrollerbase.h
  ${inc}/controller/view_controller/mindmapcontroller.h
  ${inc}/controller/view_controller/mindmapcontrollerbase.h
  ${inc}/controller/view_controller/notecontroller.h
  ${inc}/controller/view_controller/sharednotecontroller.h
  ${inc}/controller/view_controller/vectorialmapcontroller.h
  ${inc}/controller/view_controller/webpagecontroller.h
  ${inc}/controller/view_controller/selectconnprofilecontroller.h
  ${inc}/controller/view_controller/sidemenucontroller.h
  ${inc}/data/campaign.h
  ${inc}/data/campaigneditor.h
  ${inc}/data/campaignmanager.h
  ${inc}/data/cleveruri.h
  ${inc}/data/cleverurimimedata.h
  ${inc}/data/diceshortcut.h
  ${inc}/data/localpersonmodel.h
  ${inc}/data/media.h
  ${inc}/data/chatroom.h
  ${inc}/data/chatroomfactory.h
  ${inc}/data/rolisteammimedata.h
  ${inc}/data/rolisteamtheme.h
  ${inc}/data/shortcutmodel.h
  ${inc}/media/mediafactory.h
  ${inc}/media/mediatype.h
  ${inc}/model/colormodel.h
  ${inc}/model/statemodel.h
  ${inc}/model/actiononlistmodel.h
  ${inc}/model/charactermodel.h
  ${inc}/model/characterstatemodel.h
  ${inc}/model/historymodel.h
  ${inc}/model/contentmodel.h
  ${inc}/model/dicealiasmodel.h
  ${inc}/model/genericmodel.h
  ${inc}/model/languagemodel.h
  ${inc}/model/mediafilteredmodel.h
  ${inc}/model/mediamodel.h
  ${inc}/model/musicmodel.h
  ${inc}/model/nonplayablecharactermodel.h
  ${inc}/model/palettemodel.h
  ${inc}/model/participantsmodel.h
  ${inc}/model/participantmodel.h
  ${inc}/model/patternmodel.h
  ${inc}/model/playermodel.h
  ${inc}/model/playerproxymodel.h
  ${inc}/model/profilemodel.h
  ${inc}/model/remoteplayermodel.h
  ${inc}/model/singlecontenttypemodel.h
  ${inc}/model/thememodel.h
  ${inc}/model/vmapitemmodel.h
  ${inc}/model/messagemodel.h
  ${inc}/model/filterinstantmessagingmodel.h
  ${inc}/model/messagemodel.h
  ${inc}/model/chatroomsplittermodel.h
  ${inc}/model/filteredplayermodel.h
  ${inc}/model/instantmessagingmodel.h
  ${inc}/model/proxystatemodel.h
  ${inc}/model/filteredcharactermodel.h
  ${inc}/preferences/preferenceslistener.h
  ${inc}/preferences/preferencesmanager.h
  ${inc}/services/ipchecker.h
  ${inc}/services/tipchecker.h
  ${inc}/services/updatechecker.h
  ${inc}/session/sessionitemmodel.h
  ${inc}/undoCmd/addfogofwarchangecommand.h
  ${inc}/undoCmd/addlocalcharactercommand.h
  ${inc}/undoCmd/addnonplayablecharactercommand.h
  ${inc}/undoCmd/addvmapitem.h
  ${inc}/undoCmd/changecoloritem.h
  ${inc}/undoCmd/changesizevmapitem.h
  ${inc}/undoCmd/anchorvmapitemcommand.h
  ${inc}/undoCmd/deletevmapitem.h
  ${inc}/undoCmd/hideotherlayercommand.h
  ${inc}/undoCmd/importdatamedia.h
  ${inc}/undoCmd/importimagestate.h
  ${inc}/undoCmd/importmedia.h
  ${inc}/undoCmd/newmediacontroller.h
  ${inc}/undoCmd/openmediacontroller.h
  ${inc}/undoCmd/renamecampaignmedia.h
  ${inc}/undoCmd/removelocalcharactercommand.h
  ${inc}/undoCmd/removemediacontrollercommand.h
  ${inc}/undoCmd/showtransparentitemcommand.h
  ${inc}/undoCmd/rollinitcommand.h
  ${inc}/undoCmd/dupplicatevmapitem.h
  ${inc}/updater/controller/audioplayerupdater.h
  ${inc}/updater/media/campaignupdater.h
  ${inc}/updater/media/charactersheetupdater.h
  ${inc}/updater/media/genericupdater.h
  ${inc}/updater/media/instantmessagingupdater.h
  ${inc}/updater/media/mediaupdaterinterface.h
  ${inc}/updater/media/mindmapupdater.h
  ${inc}/updater/media/sharednotecontrollerupdater.h
  ${inc}/updater/media/vmapupdater.h
  ${inc}/updater/media/webviewupdater.h
  ${inc}/updater/vmapitem/characteritemupdater.h
  ${inc}/updater/vmapitem/ellipsecontrollerupdater.h
  ${inc}/updater/vmapitem/imagecontrollerupdater.h
  ${inc}/updater/vmapitem/linecontrollerupdater.h
  ${inc}/updater/vmapitem/pathcontrollerupdater.h
  ${inc}/updater/vmapitem/rectcontrollerupdater.h
  ${inc}/updater/vmapitem/textcontrollerupdater.h
  ${inc}/updater/vmapitem/vmapitemcontrollerupdater.h
  ${inc}/updater/vmapitem/sightupdater.h
  ${inc}/utils/insertionsortedmap.h
  ${inc}/utils/countdownobject.h
  ${inc}/utils/HashHelper.h
  ${inc}/worker/autosavecontroller.h
  ${inc}/worker/convertionhelper.h
  ${inc}/worker/fileserializer.h
  ${inc}/worker/iohelper.h
  ${inc}/worker/messagehelper.h
  ${inc}/worker/modelhelper.h
  ${inc}/worker/utilshelper.h
  ${inc}/worker/vectorialmapmessagehelper.h
  ${inc}/worker/characterfinder.h)


set(SOURCES
  src/controller/antagonistboardcontroller.cpp
  src/controller/contentcontroller.cpp
  src/controller/gamecontroller.cpp
  src/controller/instantmessagingcontroller.cpp
  src/controller/item_controllers/characteritemcontroller.cpp
  src/controller/item_controllers/ellipsecontroller.cpp
  src/controller/item_controllers/gridcontroller.cpp
  src/controller/item_controllers/imageitemcontroller.cpp
  src/controller/item_controllers/linecontroller.cpp
  src/controller/item_controllers/pathcontroller.cpp
  src/controller/item_controllers/rectcontroller.cpp
  src/controller/item_controllers/sightcontroller.cpp
  src/controller/item_controllers/textcontroller.cpp
  src/controller/item_controllers/visualitemcontroller.cpp
  src/controller/item_controllers/vmapitemfactory.cpp
  src/controller/networkcontroller.cpp
  src/controller/playercontroller.cpp
  src/controller/preferencescontroller.cpp
  src/controller/audioplayercontroller.cpp
  src/controller/view_controller/charactersheetcontroller.cpp
  src/controller/view_controller/sidemenucontroller.cpp
  src/controller/view_controller/pdfcontroller.cpp
  src/controller/view_controller/imagecontroller.cpp
  src/controller/view_controller/imageselectorcontroller.cpp
  src/controller/view_controller/mediacontrollerbase.cpp
  src/controller/view_controller/mindmapcontroller.cpp
  src/controller/view_controller/mindmapcontrollerbase.cpp
  src/controller/view_controller/notecontroller.cpp
  src/controller/view_controller/sharednotecontroller.cpp
  src/controller/view_controller/vectorialmapcontroller.cpp
  src/controller/view_controller/webpagecontroller.cpp
  src/controller/view_controller/selectconnprofilecontroller.cpp
  src/controller/audiocontroller.cpp
  src/data/campaign.cpp
  src/data/campaigneditor.cpp
  src/data/campaignmanager.cpp
  src/data/cleveruri.cpp
  src/data/cleverurimimedata.cpp
  src/data/diceshortcut.cpp
  src/data/localpersonmodel.cpp
  src/data/media.cpp
  src/data/chatroom.cpp
  src/data/chatroomfactory.cpp
  src/data/rolisteammimedata.cpp
  src/data/rolisteamtheme.cpp
  src/data/shortcutmodel.cpp
  src/media/mediafactory.cpp
  src/model/colormodel.cpp
  src/model/statemodel.cpp
  src/model/messagemodel.cpp
  src/model/filteredcharactermodel.cpp
  src/model/filterinstantmessagingmodel.cpp
  src/model/messagemodel.cpp
  src/model/chatroomsplittermodel.cpp
  src/model/filteredplayermodel.cpp
  src/model/instantmessagingmodel.cpp
  src/model/actiononlistmodel.cpp
  src/model/charactermodel.cpp
  src/model/characterstatemodel.cpp
  src/model/proxystatemodel.cpp
  src/model/contentmodel.cpp
  src/model/dicealiasmodel.cpp
  src/model/languagemodel.cpp
  src/model/mediafilteredmodel.cpp
  src/model/mediamodel.cpp
  src/model/historymodel.cpp
  src/model/musicmodel.cpp
  src/model/nonplayablecharactermodel.cpp
  src/model/palettemodel.cpp
  src/model/participantsmodel.cpp
  src/model/participantmodel.cpp
  src/model/patternmodel.cpp
  src/model/playermodel.cpp
  src/model/playerproxymodel.cpp
  src/model/profilemodel.cpp
  src/model/genericmodel.cpp
  src/model/remoteplayermodel.cpp
  src/model/singlecontenttypemodel.cpp
  src/model/thememodel.cpp
  src/model/vmapitemmodel.cpp
  src/preferences/preferenceslistener.cpp
  src/preferences/preferencesmanager.cpp
  src/services/ipchecker.cpp
  src/services/tipchecker.cpp
  src/services/updatechecker.cpp
  src/session/sessionitemmodel.cpp
  src/undoCmd/addfogofwarchangecommand.cpp
  src/undoCmd/addlocalcharactercommand.cpp
  src/undoCmd/addnonplayablecharactercommand.cpp
  src/undoCmd/anchorvmapitemcommand.cpp
  src/undoCmd/addvmapitem.cpp
  src/undoCmd/changecoloritem.cpp
  src/undoCmd/changesizevmapitem.cpp
  src/undoCmd/deletevmapitem.cpp
  src/undoCmd/hideotherlayercommand.cpp
  src/undoCmd/importdatamedia.cpp
  src/undoCmd/importimagestate.cpp
  src/undoCmd/importmedia.cpp
  src/undoCmd/newmediacontroller.cpp
  src/undoCmd/openmediacontroller.cpp
  src/undoCmd/renamecampaignmedia.cpp
  src/undoCmd/rollinitcommand.cpp
  src/undoCmd/removelocalcharactercommand.cpp
  src/undoCmd/removemediacontrollercommand.cpp
  src/undoCmd/showtransparentitemcommand.cpp
  src/undoCmd/dupplicatevmapitem.cpp
  src/updater/controller/audioplayerupdater.cpp
  src/updater/media/campaignupdater.cpp
  src/updater/media/charactersheetupdater.cpp
  src/updater/media/genericupdater.cpp
  src/updater/media/instantmessagingupdater.cpp
  src/updater/media/mediaupdaterinterface.cpp
  src/updater/media/mindmapupdater.cpp
  src/updater/media/sharednotecontrollerupdater.cpp
  src/updater/media/vmapupdater.cpp
  src/updater/media/webviewupdater.cpp
  src/updater/vmapitem/characteritemupdater.cpp
  src/updater/vmapitem/ellipsecontrollerupdater.cpp
  src/updater/vmapitem/imagecontrollerupdater.cpp
  src/updater/vmapitem/linecontrollerupdater.cpp
  src/updater/vmapitem/pathcontrollerupdater.cpp
  src/updater/vmapitem/rectcontrollerupdater.cpp
  src/updater/vmapitem/textcontrollerupdater.cpp
  src/updater/vmapitem/sightupdater.cpp
  src/updater/vmapitem/vmapitemcontrollerupdater.cpp
  src/utils/countdownobject.cpp
  src/worker/autosavecontroller.cpp
  src/worker/convertionhelper.cpp
  src/worker/fileserializer.cpp
  src/worker/iohelper.cpp
  src/worker/messagehelper.cpp
  src/worker/modelhelper.cpp
  src/worker/utilshelper.cpp
  src/worker/vectorialmapmessagehelper.cpp
  src/worker/characterfinder.cpp)

add_library(core SHARED ${SOURCES} ${HEADERS} ${pdfSource} ${QRC_SOURCES})
set_target_properties(core PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_compile_definitions(core PRIVATE CORE_LIBRARY)
target_compile_definitions(core PUBLIC _USE_MATH_DEFINES)
target_link_libraries(core
  PUBLIC
    Qt6::Core
    Qt6::Network
    Qt6::Multimedia
    Qt6::Concurrent
  PRIVATE
    diceparser_qobject
    diceparser_shared
    rmindmap
    common_core
    common_qml
    charactersheet
    charactersheet_widgets
    instantmessaging
    utils
    network)

target_include_directories(core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)


message(${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_CURRENT_BINARY_DIR})

if("${CAMKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/core_autogen/mocs_compilation.cpp" PROPERTIES COMPILE_FLAGS "-Wno-undefined-reinterpret-cast -Wno-useless-cast")
    set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/network_autogen/mocs_compilation.cpp" PROPERTIES COMPILE_FLAGS "-Wno-undefined-reinterpret-cast -Wno-useless-cast")
endif()

include(install_helper)
rinstallLib(core libraries)
rinstallLib(network libraries)

