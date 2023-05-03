cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Gui Widgets Network)

add_executable(tst_noteedit tst_noteedittest.cpp ../assets/resources.qrc)
target_link_libraries(tst_noteedit PUBLIC Qt6::Core Qt6::Gui Qt6::Test Qt6::Widgets Qt6::Network PRIVATE core rwidgets instantmessaging common_core common_widget common_qml helper utils)
target_include_directories(tst_noteedit PRIVATE ../../libraries)
target_include_directories(tst_noteedit  PUBLIC ../helper )
add_test(NAME tst_noteedit COMMAND tst_noteedit)
