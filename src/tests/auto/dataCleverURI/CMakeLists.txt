cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_cleveruri tst_datacleveruritest.cpp ../assets/resources.qrc)
target_link_libraries(tst_cleveruri PUBLIC Qt6::Core Qt6::Test PRIVATE core core rwidgets instantmessaging common_core common_widget common_qml)
add_test(NAME tst_cleveruri COMMAND tst_cleveruri)
