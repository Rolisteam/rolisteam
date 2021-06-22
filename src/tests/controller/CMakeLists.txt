cmake_minimum_required(VERSION 3.10)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_REQUIRED_VERSION "5.12.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Gui Widgets)

add_executable(tst_imagecontroller tst_imagecontrollertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_imagecontroller PUBLIC Qt5::Core Qt5::Test Qt5::Widgets Qt5::Gui PRIVATE core instantmessaging common_core common_widget common_qml)

add_executable(tst_charactersheetcontroller tst_charactersheetcontrollertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_charactersheetcontroller PUBLIC Qt5::Core Qt5::Test Qt5::Widgets Qt5::Gui PRIVATE core instantmessaging common_core common_widget common_qml)

add_executable(tst_vectorialmapcontroller tst_vectorialmapcontrollertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_vectorialmapcontroller PUBLIC Qt5::Core Qt5::Test Qt5::Widgets Qt5::Gui PRIVATE core instantmessaging common_core common_widget common_qml)
