cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_dicealiasmodel tst_dicealiasmodel.cpp)
target_link_libraries(tst_dicealiasmodel PUBLIC Qt6::Core Qt6::Test PRIVATE core diceparser_shared core rwidgets instantmessaging common_core common_widget common_qml)
add_test(NAME tst_dicealiasmodel COMMAND tst_dicealiasmodel)
