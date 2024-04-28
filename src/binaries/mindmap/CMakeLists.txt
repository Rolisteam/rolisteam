cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core LinguistTools Widgets Gui Concurrent Quick Pdf QuickWidgets PdfWidgets WebEngineWidgets PrintSupport WebEngineCore Widgets)


set(documentation README.md)

set(ressources  ${CMAKE_SOURCE_DIR}/resources/rcse/rcse_resources.qrc ${CMAKE_SOURCE_DIR}/rolisteam.qrc)


set(sources
  main.cpp
  maincontroller.cpp
  maincontroller.h
)

set(headers
)


set(qml_files main.qml)


set(RC_FILE ${PROJECT_SOURCE_DIR}/resources/mindmap/logo/mindmap.rc)

add_executable(mindmapbin WIN32 ${sources} ${headers} ${UI_files} ${ressources} ${RC_FILE})
qt_add_qml_module(mindmapbin
URI
    mindmapmod
RESOURCE_PREFIX
    mindmap
QML_FILES
    Main.qml
)
set_target_properties(mindmapbin  PROPERTIES OUTPUT_NAME mindmap CLEAN_DIRECT_OUTPUT 1)
set_target_properties(mindmapbin PROPERTIES WIN32_EXECUTABLE on MACOSX_BUNDLE on)

qt6_add_resources(mindmapbin mindmaprsrc
        PREFIX "/mindmap/qml"
        FILES ${qml_files})

target_link_libraries(mindmapbin
  PUBLIC
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Widgets
    Qt6::Concurrent
  PRIVATE
    core
    rmindmap
    common_qml
    qml_views
    qml_components
    utils
    instantmessaging
    diceparser_shared
    diceparser_qobject
    charactersheet
)

target_include_directories(mindmapbin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_SOURCE_DIR}/
    ${CMAKE_SOURCE_DIR}/
    ${CMAKE_BINARY_DIR})


include(install_helper)
rinstallRT(mindmapbin mindmapComp)
