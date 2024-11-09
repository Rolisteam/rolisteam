cmake_minimum_required(VERSION 3.16)

project(utils VERSION 1.0.0 DESCRIPTION "Utilitary component")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Add a compiler flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Network Test Qml)


set(public_utils_header
    include/utils/iohelper.h
    include/utils/networkdownloader.h
    include/utils/mappinghelper.h

    include/utils/utils_global.h
    include/utils/directaccessdatastructure.h
)

set(utils_src
    src/iohelper.cpp
    src/networkdownloader.cpp
    src/mappinghelper.cpp
)


add_library(utils SHARED ${public_utils_header} ${utils_src})
qt_add_qml_module(utils
URI Utils
RESOURCE_PREFIX
    utils
SOURCES
    include/utils/mappinghelper.h
    src/mappinghelper.cpp
)
set_target_properties(utils PROPERTIES PUBLIC_HEADER "${public_utils_header}")
target_compile_definitions(utils PRIVATE UTILS_LIBRARY)
target_link_libraries(utils PRIVATE Qt6::Core Qt6::Gui Qt6::Network Qt6::Test Qt6::Qml)
target_include_directories(utils PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/utils>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)



include(install_helper)
rinstallLib(utils libraries)
