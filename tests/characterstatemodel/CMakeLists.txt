cmake_minimum_required(VERSION 3.10)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "5.12.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_characterstatemodel tst_characterstatemodel.cpp)
target_link_libraries(tst_characterstatemodel PUBLIC Qt5::Core Qt5::Test PRIVATE core)
