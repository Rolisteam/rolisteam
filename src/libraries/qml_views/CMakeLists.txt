cmake_minimum_required(VERSION 3.15)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(QRC_SOURCES viewsqml.qrc)

add_library(qml_views SHARED ${QRC_SOURCES})
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Svg)
target_link_libraries(qml_views PUBLIC Qt5::Core Qt5::Svg)
