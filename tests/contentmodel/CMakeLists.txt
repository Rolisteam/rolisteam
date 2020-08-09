cmake_minimum_required(VERSION 3.10)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

find_package(Qt5 CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_contentmodel tst_contentmodel.cpp)
target_link_libraries(tst_contentmodel PUBLIC Qt5::Core Qt5::Test PRIVATE core)
