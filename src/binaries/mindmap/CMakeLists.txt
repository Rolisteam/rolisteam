cmake_minimum_required(VERSION 3.16)
project(mindmapbin)

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

add_executable(mindmap-bin ${sources} ${headers} ${UI_files} ${ressources})
set_target_properties(mindmap-bin  PROPERTIES OUTPUT_NAME mindmap CLEAN_DIRECT_OUTPUT 1)

qt6_add_resources(mindmap-bin mindmaprsrc
        PREFIX "/mindmap/qml"
        FILES ${qml_files})

target_link_libraries(mindmap-bin
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
)

target_include_directories(mindmap-bin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_SOURCE_DIR}/
    ${CMAKE_SOURCE_DIR}/
    ${CMAKE_BINARY_DIR})


include(install_helper)
rinstallRT(mindmap-bin mindmapComp)
