cmake_minimum_required(VERSION 3.16)

project(rolisteamd)


option(UPDATE_TRANSLATIONS "update Translation" OFF)
MESSAGE(STATUS "UPDATE TRANSLATIONS: ${UPDATE_TRANSLATIONS}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt6Core)
find_package(Qt6Network)
find_package(Qt6Gui)
set(EXECUTABLE_OUTPUT_PATH bin/)

include_directories(${Qt6Core_INCLUDES} ${Qt6Gui_INCLUDES} ${Qt6Network_INCLUDES} ../)
add_definitions(${Qt6Core_DEFINITIONS} ${Qt6Gui_DEFINITIONS} ${Qt6Network_DEFINITIONS})

set(MODE "cli")

ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    # Other flags
)

SET( rolisteamd_sources
    main.cpp
    tcpclient.cpp
    servermanager.cpp
    rolisteamdaemon.cpp
    connectionprofile.cpp
    channelmodel.cpp
    )

