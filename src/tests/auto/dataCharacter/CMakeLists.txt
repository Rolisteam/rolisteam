cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_character tst_datacharactertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_character PUBLIC Qt6::Core Qt6::Test PRIVATE core core rwidgets instantmessaging common_core common_widget common_qml network helper)
target_include_directories(tst_character PUBLIC ../helper)
add_test(NAME tst_character COMMAND tst_character)
