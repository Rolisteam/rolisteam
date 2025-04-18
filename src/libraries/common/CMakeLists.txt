cmake_minimum_required(VERSION 3.16)

project(common VERSION 1.0.0 DESCRIPTION "Common Elements")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add a compiler flag
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(QT_REQUIRED_VERSION "6.3.0")
set(QT_VERSION_MAJOR "6")
find_package(Qt${QT_VERSION_MAJOR} ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets Network Qml Gui)


set(common_core_public_headers include/common/common_global.h
    include/common/remotelogcontroller.h
    include/common/logcontroller.h
    include/common/common_types.h
    include/common/logcategory.h
    include/common/uploadlogtoserver.h)

set(common_core_src
  task/uploadlogtoserver.cpp
  controller/remotelogcontroller.cpp
  controller/logcontroller.cpp
)



add_library(common_core SHARED ${common_core_src} ${common_core_public_headers})
target_compile_definitions(common_core PRIVATE COMMON_LIBRARY)
set_target_properties(common_core PROPERTIES PUBLIC_HEADER "${common_core_public_headers}")
target_link_libraries(common_core PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)
target_include_directories(common_core
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/task
        ${CMAKE_CURRENT_SOURCE_DIR}/controller
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

#===========================================================

set(common_widget_public_header
include/common_widgets/logpanel.h
include/common_widgets/colorbutton.h
include/common_widgets/common_widgets_global.h
include/common_widgets/busyindicatordialog.h
)
set(common_widget_src
widgets/logpanel.cpp
widgets/colorbutton.cpp
widgets/busyindicatordialog.cpp
)

add_library(common_widget SHARED ${common_widget_public_header} ${common_widget_src})
target_compile_definitions(common_widget PRIVATE COMMON_WIDGET_LIBRARY)
set_target_properties(common_widget PROPERTIES PUBLIC_HEADER "${common_widget_public_header}")
target_link_libraries(common_widget PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets common_core)
target_include_directories(common_widget
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/widgets
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)


#===========================================================

set(common_qml_public_header include/common_qml/common_qml_global.h include/common_qml/theme.h)
set(common_qml_src controller/theme.cpp)

add_library(common_qml SHARED ${common_qml_public_header} ${common_qml_src})
qt_add_qml_module(common_qml
URI
    Customization
VERSION
    1.0
SOURCES
    include/common_qml/theme.h
    controller/theme.cpp
)

target_compile_definitions(common_qml PRIVATE COMMON_QML_LIBRARY)
set_target_properties(common_qml PROPERTIES PUBLIC_HEADER "${common_qml_public_header}")
target_link_libraries(common_qml PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Qml Qt${QT_VERSION_MAJOR}::Gui)
target_include_directories(common_qml
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/controller
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/common_qml>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

include(install_helper)
rinstallLib(common_qml libraries)
rinstallLib(common_widget libraries)
rinstallLib(common_core libraries)
