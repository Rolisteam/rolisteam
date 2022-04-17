cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Svg Quick Qml)

set(QRC_SOURCES)
set(qml_compo_sources src/avatarprovider.cpp)
set(qml_compo_headers include/qml_components/avatarprovider.h
include/qml_components/qmlcomponents_global.h)

add_library(qml_components SHARED ${QRC_SOURCES} ${qml_compo_headers} ${qml_compo_sources})
set_target_properties(qml_components PROPERTIES PUBLIC_HEADER "${qml_compo_headers}")
target_compile_definitions(qml_components PRIVATE QMLCOMPONENTS_LIBRARY)
target_link_libraries(qml_components PUBLIC Qt6::Core Qt6::Svg Qt6::Quick Qt6::Qml PRIVATE core util)

target_include_directories(qml_components PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
