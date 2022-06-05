cmake_minimum_required(VERSION 3.16)

include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Widgets Qml Quick)

set(RESOURCE_FILES ../../../rolisteam.qrc source.qrc)

add_executable(instantmessaging_manual main.cpp message.h message.cpp ${RESOURCE_FILES})
target_link_libraries(instantmessaging_manual PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Qml Qt6::Quick PRIVATE core instantmessaging rwidgets common_core common_widget common_qml qml_views network charactersheet_widgets qml_components)
add_test(NAME instantmessaging_manual COMMAND instantmessaging_manual)
