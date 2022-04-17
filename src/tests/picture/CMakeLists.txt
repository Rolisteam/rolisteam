cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Gui Widgets)

add_executable(tst_picture tst_picturetest.cpp ../assets/resources.qrc)
target_link_libraries(tst_picture PUBLIC Qt6::Core Qt6::Gui Qt6::Test Qt6::Widgets PRIVATE core rwidgets instantmessaging common_core common_widget common_qml)
target_include_directories(tst_picture PRIVATE ../../libraries)
# rwidgets common_core common_widget common_qml  charactersheet_widgets diceparser_qobject qml_views rmindmap
