cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Gui Widgets)

add_executable(tst_theme tst_rolisteamtheme.cpp)
target_link_libraries(tst_theme PUBLIC Qt6::Core Qt6::Gui Qt6::Test Qt6::Widgets PRIVATE core instantmessaging common_core common_widget common_qml)
