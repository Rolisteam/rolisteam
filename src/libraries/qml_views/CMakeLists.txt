cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Svg Quick Qml)

set(QRC_SOURCES viewsqml.qrc)
set(qml_views_sources
  src/image_selector.cpp
)

set(qml_views_headers
  include/qml_views/qmlviews_global.h
  include/qml_views/image_selector.h
)

add_library(qml_views SHARED ${QRC_SOURCES} ${qml_views_headers} ${qml_views_sources})
set_target_properties(qml_views PROPERTIES PUBLIC_HEADER "${qml_views_headers}")
target_compile_definitions(qml_views PRIVATE QMLVIEWS_LIBRARY)
target_link_libraries(qml_views PUBLIC Qt6::Core Qt6::Svg Qt6::Quick Qt6::Qml)

target_include_directories(qml_views PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)


include(install_helper)
rinstallLib(qml_views libraries)
