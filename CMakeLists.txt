cmake_minimum_required(VERSION 3.10)

project(rolisteam)

add_definitions(-DVERSION_MAJOR=1 -DVERSION_MIDDLE=9 -DVERSION_MINOR=0)

add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(server)
#add_subdirectory(android)

option(TESTS "compile tests" OFF) #Off by default

if (TESTS)
add_subdirectory(tests)
endif()


set(documentation README.md HelpMe.md)
