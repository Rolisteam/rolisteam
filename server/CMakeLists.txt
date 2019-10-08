cmake_minimum_required(VERSION 3.10)


project(roliserver)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../core/src)

set(SOURCES main.cpp )
set(HEADERS )

set(QT_REQUIRED_VERSION "5.12.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Network)


add_executable(roliserver ${SOURCES} ${HEADERS})


target_link_libraries(roliserver PUBLIC Qt5::Core Qt5::Network PRIVATE network)
