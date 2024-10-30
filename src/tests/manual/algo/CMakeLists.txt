cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Widgets)

add_executable(tst_algo tst_algo.cpp)
target_link_libraries(tst_algo PUBLIC Qt6::Core Qt6::Test Qt6::Widgets PRIVATE core )

