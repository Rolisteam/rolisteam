cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_REQUIRED_VERSION "5.12.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Gui Widgets)

add_executable(tst_imagecontroller tst_imagecontrollertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_imagecontroller PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Gui PRIVATE core instantmessaging common_core common_widget common_qml)

add_executable(tst_charactersheetcontroller tst_charactersheetcontrollertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_charactersheetcontroller PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Gui PRIVATE core instantmessaging common_core common_widget common_qml)

add_executable(tst_vectorialmapcontroller tst_vectorialmapcontrollertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_vectorialmapcontroller PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Gui PRIVATE core instantmessaging common_core common_widget common_qml)
