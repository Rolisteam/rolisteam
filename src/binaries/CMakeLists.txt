option(QML_CLI "enable compilation of QML client" OFF)

if (QML_CLI)
add_subdirectory(android)
endif()
add_subdirectory(server)
add_subdirectory(widget)
add_subdirectory(rcse)
