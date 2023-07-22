set(TEST_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

configure_file(test_root_path.h.in test_root_path.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(manual)
add_subdirectory(communication)
add_subdirectory(auto)

