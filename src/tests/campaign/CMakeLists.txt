cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)

set(RESOURCE_FILES ../assets/resources.qrc)
add_executable(tst_campaign tst_campaign.cpp ${RESOURCE_FILES})
target_link_libraries(tst_campaign PUBLIC Qt6::Core Qt6::Test PRIVATE diceparser_qobject diceparser_shared core rwidgets instantmessaging common_core common_widget common_qml util network)
