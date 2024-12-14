cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Widgets)

add_executable(tst_sheet tst_sheet.cpp ../assets/resources.qrc
    ../../../binaries/rcse/fieldmodel.h
    ../../../binaries/rcse/fieldmodel.cpp)
target_link_libraries(tst_sheet PUBLIC Qt6::Core Qt6::Test Qt6::Widgets PRIVATE core rwidgets rmindmap common_core common_widget common_qml helper utils diceparser_shared diceparser_qobject charactersheet charactersheet_formula charactersheet_widgets)
target_include_directories(tst_sheet PUBLIC ../helper ../../../binaries/)
add_test(NAME tst_vmap COMMAND tst_vmap)

