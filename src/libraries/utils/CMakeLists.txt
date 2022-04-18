cmake_minimum_required(VERSION 3.16)

project(utils VERSION 1.0.0 DESCRIPTION "Utilitary component")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add a compiler flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Gui)


set(public_util_header
    include/iohelper.h
    include/util_global.h
)

set(util_src
    src/iohelper.cpp
)


add_library(util SHARED ${public_util_header} ${util_src})
set_target_properties(util PROPERTIES PUBLIC_HEADER "${public_util_header}")
target_compile_definitions(util PRIVATE UTIL_LIBRARY)
target_link_libraries(util PRIVATE Qt6::Core Qt6::Gui)
target_include_directories(util PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)


  include(GNUInstallDirs)

  install(TARGETS util
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
