cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Widgets)

add_executable(tst_vmap tst_vmap.cpp)
target_link_libraries(tst_vmap PUBLIC Qt6::Core Qt6::Test Qt6::Widgets PRIVATE core rwidgets instantmessaging common_core common_widget common_qml diceparser_shared diceparser_qobject)
add_test(NAME tst_vmap COMMAND tst_vmap)


add_executable(tst_vmapitemcontroller tst_vmapitemcontroller.cpp)
target_link_libraries(tst_vmapitemcontroller PUBLIC Qt6::Core Qt6::Test Qt6::Widgets PRIVATE core rwidgets instantmessaging common_core common_widget common_qml helper network diceparser_shared diceparser_qobject)
target_include_directories(tst_vmapitemcontroller PUBLIC ../helper)
add_test(NAME tst_vmapitemcontroller COMMAND tst_vmapitemcontroller)

