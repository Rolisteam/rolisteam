cmake_minimum_required(VERSION 3.16)

project(common VERSION 1.0.0 DESCRIPTION "Common Elements")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add a compiler flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(QT_REQUIRED_VERSION "5.15.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets Network Qml Gui)


set(common_core_header
  task/uploadlogtoserver.h
  controller/remotelogcontroller.h
  controller/logcontroller.h
)



set(common_core_src
  task/uploadlogtoserver.cpp
  controller/remotelogcontroller.cpp
  controller/logcontroller.cpp
)



add_library(common_core SHARED ${common_core_header} ${common_core_src})
target_link_libraries(common_core PUBLIC Qt5::Core Qt5::Network)
target_include_directories(common_core PRIVATE ../)


set(common_widget_header
widgets/logpanel.h
widgets/colorbutton.h
)
set(common_widget_src
widgets/logpanel.cpp
widgets/colorbutton.cpp
)

add_library(common_widget SHARED ${common_widget_header} ${common_widget_src})
target_link_libraries(common_widget PUBLIC Qt5::Core Qt5::Widgets)
target_include_directories(common_widget PRIVATE ../)


set(common_qml_header controller/theme.h)
set(common_qml_src controller/theme.cpp)

add_library(common_qml SHARED ${common_qml_header} ${common_qml_src})
target_link_libraries(common_qml PUBLIC Qt5::Core Qt5::Qml Qt5::Gui)
target_include_directories(common_qml PRIVATE ../)
