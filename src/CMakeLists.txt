
option(TESTS "enable tests compilation" OFF)


add_subdirectory(libraries)
add_subdirectory(binaries)


if (TESTS)
enable_testing()
add_subdirectory(tests)
endif()
