cmake_minimum_required(VERSION 3.10)


project(roliserver)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


include_directories(../core/src)
set(QT_REQUIRED_VERSION "5.13.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Network DBus)

if(UNIX AND NOT APPLE)
    message("On linux")
    qt5_generate_dbus_interface(rolisteamdaemon.h org.rolisteam.rolisteamdaemon.xml OPTIONS -S -M)
    qt5_add_dbus_adaptor(DBUS_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/org.rolisteam.rolisteamdaemon.xml rolisteamdaemon.h RolisteamDaemon)
    message("Dbus adaptor here: ${DBUS_SOURCES}")
endif()

set(SOURCES main.cpp  rolisteamdaemon.cpp )
set(HEADERS rolisteamdaemon.h )






add_executable(roliserver ${SOURCES} ${HEADERS} ${DBUS_SOURCES})


target_link_libraries(roliserver PUBLIC Qt5::Core Qt5::Network Qt5::DBus PRIVATE network)
