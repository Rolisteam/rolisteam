cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Widgets)

add_executable(comm_vmap tst_vmap_with_item.cpp ${CMAKE_SOURCE_DIR}/src/binaries/server/rolisteamdaemon.cpp ${CMAKE_SOURCE_DIR}/src/binaries/server/rolisteamdaemon.h ${TEST_ROOT_DIR}/auto/assets/resources.qrc)
target_include_directories(comm_vmap PUBLIC ${CMAKE_SOURCE_DIR}/src/binaries/server/ )
target_link_libraries(comm_vmap PUBLIC Qt6::Core Qt6::Test Qt6::Widgets PRIVATE core rwidgets common_core common_widget common_qml instantmessaging diceparser_qobject diceparser_shared utils qml_views qml_components network dice3d)

