cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(QRC_SOURCES viewsqml.qrc)
set(qml_views_sources src/image_selector.cpp)
set(qml_views_headers src/image_selector.h)


add_library(qml_views SHARED ${QRC_SOURCES} ${qml_views_headers} ${qml_views_sources})
find_package(Qt6 COMPONENTS Core Svg Quick Qml REQUIRED)
target_link_libraries(qml_views PUBLIC Qt6::Core Qt6::Svg Qt6::Quick Qt6::Qml)
