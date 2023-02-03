cmake_minimum_required(VERSION 3.16)
project(helper VERSION 1.0.0 DESCRIPTION "Helper component for unit tests")

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Test HttpServer)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_library(helper SHARED helper.cpp helper.h ../assets/resources.qrc )
target_link_libraries(helper PUBLIC Qt6::Core Qt6::Test  Qt6::HttpServer PRIVATE core utils)

target_include_directories(helper
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
