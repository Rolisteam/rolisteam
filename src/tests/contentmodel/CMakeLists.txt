cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

find_package(Qt6 CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_contentmodel tst_contentmodel.cpp)
target_link_libraries(tst_contentmodel PUBLIC Qt6::Core Qt6::Test PRIVATE core core rwidgets instantmessaging common_core common_widget common_qml)
