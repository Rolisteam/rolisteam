
option(TESTS "enable tests compilation" OFF)


add_subdirectory(libraries)
add_subdirectory(binaries)


if (BUILD_TESTING)
add_subdirectory(tests)
endif()
