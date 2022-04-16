cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Gui Widgets)

add_executable(tst_audioplayer tst_audioplayer.cpp ../assets/resources.qrc)
target_link_libraries(tst_audioplayer PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Test PRIVATE core rwidgets instantmessaging common_core common_widget common_qml)
