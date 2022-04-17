cmake_minimum_required(VERSION 3.16)
project(helper VERSION 1.0.0 DESCRIPTION "Helper component for unit tests")

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core)


add_library(helper SHARED helper.cpp helper.h)
target_link_libraries(helper PUBLIC Qt6::Core PRIVATE core)


