cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Gui Widgets StateMachine)

add_executable(tst_imagecontroller tst_imagecontrollertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_imagecontroller PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Gui PRIVATE core instantmessaging common_core common_widget common_qml)
add_test(NAME tst_imagecontroller COMMAND tst_imagecontroller)

add_executable(tst_charactersheetcontroller tst_charactersheetcontrollertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_charactersheetcontroller PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Gui PRIVATE charactersheet core instantmessaging common_core common_widget common_qml)
add_test(NAME tst_charactersheetcontroller COMMAND tst_charactersheetcontroller)

add_executable(tst_vectorialmapcontroller tst_vectorialmapcontrollertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_vectorialmapcontroller PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Gui PRIVATE rmindmap core instantmessaging common_core common_widget common_qml network helper)
target_include_directories(tst_vectorialmapcontroller  PUBLIC ../helper ../../libraries/mindmap/include)
add_test(NAME tst_vectorialmapcontroller COMMAND tst_vectorialmapcontroller)


add_executable(tst_profilecontroller tst_profilecontroller.cpp ../assets/resources.qrc)
target_link_libraries(tst_profilecontroller PUBLIC Qt6::Core Qt6::Test Qt6::StateMachine PRIVATE core instantmessaging common_core common_widget common_qml helper network diceparser_qobject diceparser_shared)
target_include_directories(tst_profilecontroller  PUBLIC ../helper )
add_test(NAME tst_profilecontroller COMMAND tst_profilecontroller)
