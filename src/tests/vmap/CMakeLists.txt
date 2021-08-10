cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "5.12.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Widgets)

add_executable(tst_vmap tst_vmap.cpp)
target_link_libraries(tst_vmap PUBLIC Qt5::Core Qt5::Test Qt5::Widgets PRIVATE core rwidgets instantmessaging common_core common_widget common_qml )
