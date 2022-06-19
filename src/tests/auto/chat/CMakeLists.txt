cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Widgets Qml Quick)

add_executable(tst_chat tst_chat.cpp)
target_link_libraries(tst_chat PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Qml Qt6::Quick PRIVATE core instantmessaging common_core common_widget common_qml qml_components rwidgets network)
add_test(NAME tst_chat COMMAND tst_chat)
