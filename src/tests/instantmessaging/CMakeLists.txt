cmake_minimum_required(VERSION 3.16)

include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "5.15.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Widgets Qml Quick)

set(RESOURCE_FILES ../../../rolisteam.qrc source.qrc)

add_executable(instantmessaging_manual main.cpp message.h message.cpp ${RESOURCE_FILES})
target_link_libraries(instantmessaging_manual PUBLIC Qt5::Core Qt5::Test Qt5::Widgets Qt5::Qml Qt5::Quick PRIVATE core instantmessaging rwidgets common_core common_widget common_qml qml_views  charactersheet_widgets)
