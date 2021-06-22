cmake_minimum_required(VERSION 3.10)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "5.12.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_mindmap tst_mindmap.cpp)
target_link_libraries(tst_mindmap PUBLIC Qt5::Core Qt5::Test PRIVATE core rmindmap rwidgets instantmessaging common_core common_widget common_qml)
