cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Widgets)

add_executable(tst_vmap tst_vmap.cpp ../assets/resources.qrc)
target_link_libraries(tst_vmap PUBLIC Qt6::Core Qt6::Test Qt6::Widgets PRIVATE core rwidgets instantmessaging common_core common_widget common_qml helper utils diceparser_shared diceparser_qobject)
target_include_directories(tst_vmap PUBLIC ../helper)
add_test(NAME tst_vmap COMMAND tst_vmap)


add_executable(tst_characteritemctrltest tst_vmapitemcontroller.cpp ../assets/resources.qrc)
target_link_libraries(tst_characteritemctrltest PUBLIC Qt6::Core Qt6::Test Qt6::Widgets PRIVATE core rwidgets instantmessaging common_core common_widget common_qml helper network diceparser_shared diceparser_qobject utils)
target_include_directories(tst_characteritemctrltest PUBLIC ../helper)
add_test(NAME tst_characteritemctrltest COMMAND tst_characteritemctrltest)

