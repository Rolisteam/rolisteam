cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_widgets tst_widgets.cpp)
target_link_libraries(tst_widgets PUBLIC Qt6::Core Qt6::Test PRIVATE core rwidgets instantmessaging common_core common_widget common_qml charactersheet_widgets helper utils)
target_include_directories(tst_widgets  PUBLIC ../helper )
add_test(NAME tst_widgets COMMAND tst_widgets)
