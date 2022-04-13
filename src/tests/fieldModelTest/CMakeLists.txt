cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "5.12.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_fieldmodel tst_fieldmodel.cpp)
target_link_libraries(tst_fieldmodel PUBLIC Qt6::Core Qt6::Test PRIVATE core instantmessaging common_core common_widget common_qml)
