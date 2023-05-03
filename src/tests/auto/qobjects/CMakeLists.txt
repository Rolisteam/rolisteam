cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Widgets)



add_executable(tst_qobjects tst_qobjects.cpp ../assets/resources.qrc)
target_link_libraries(tst_qobjects
    PUBLIC
        Qt6::Core
        Qt6::Test
        Qt6::Widgets
    PRIVATE
        core
        rwidgets
        diceparser_qobject
        diceparser_shared
        instantmessaging
        rmindmap
        common_core
        common_widget
        common_qml
        helper
        utils
        network
        charactersheet
        charactersheet_formula
        charactersheet_widgets
        qml_components
        qml_views)

target_include_directories(tst_qobjects PUBLIC ../helper)
add_test(NAME tst_qobjects COMMAND tst_qobjects)
