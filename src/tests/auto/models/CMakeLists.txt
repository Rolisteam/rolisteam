cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Gui Widgets Network)

add_executable(tst_models tst_modelstest.cpp ../assets/resources.qrc)
target_link_libraries(tst_models
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


target_include_directories(tst_models PRIVATE ../../libraries)
target_include_directories(tst_models  PUBLIC ../helper )
# rwidgets common_core common_widget common_qml  charactersheet_widgets diceparser_qobject qml_views rmindmap
add_test(NAME tst_models COMMAND tst_models)
