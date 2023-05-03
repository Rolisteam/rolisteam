cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Gui Widgets Network)

add_executable(tst_picture tst_picturetest.cpp ../assets/resources.qrc)
target_link_libraries(tst_picture PUBLIC Qt6::Core Qt6::Gui Qt6::Test Qt6::Widgets Qt6::Network PRIVATE core rwidgets instantmessaging common_core common_widget common_qml helper utils)
target_include_directories(tst_picture PRIVATE ../../libraries)
target_include_directories(tst_picture  PUBLIC ../helper )
# rwidgets common_core common_widget common_qml  charactersheet_widgets diceparser_qobject qml_views rmindmap
add_test(NAME tst_picture COMMAND tst_picture)
