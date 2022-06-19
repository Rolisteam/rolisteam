cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core StateMachine)

add_executable(tst_channelmodel tst_channelmodel.cpp)
target_link_libraries(tst_channelmodel PUBLIC Qt6::Core Qt6::Test Qt6::StateMachine PRIVATE core rwidgets instantmessaging common_core common_widget common_qml network)
add_test(NAME tst_channelmodel COMMAND tst_channelmodel)
