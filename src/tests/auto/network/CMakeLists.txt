cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_network tst_network.cpp)
target_link_libraries(tst_network PUBLIC Qt6::Core Qt6::Test PRIVATE network core core rwidgets rmindmap instantmessaging common_core common_widget common_qml helper)
target_include_directories(tst_network PUBLIC ../helper)
add_test(NAME tst_network COMMAND tst_network)
