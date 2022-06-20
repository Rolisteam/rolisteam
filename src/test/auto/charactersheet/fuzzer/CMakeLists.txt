cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core)

add_executable(fuzzed_formula testcase.cpp)
target_link_libraries(fuzzed_formula PUBLIC Qt6::Core PRIVATE charactersheet_formula)
