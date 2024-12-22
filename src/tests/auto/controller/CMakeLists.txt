cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Gui Widgets StateMachine)

add_executable(tst_imagecontroller tst_imagecontrollertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_imagecontroller PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Gui PRIVATE core instantmessaging common_core common_widget common_qml utils helper)
target_include_directories(tst_imagecontroller PUBLIC ../helper)
add_test(NAME tst_imagecontroller COMMAND tst_imagecontroller)

add_executable(tst_pdfcontroller tst_pdfcontrollertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_pdfcontroller PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Gui PRIVATE core instantmessaging common_core common_widget common_qml utils helper)
target_include_directories(tst_pdfcontroller PUBLIC ../helper)
add_test(NAME tst_pdfcontroller COMMAND tst_pdfcontroller)

add_executable(tst_charactersheetcontroller tst_charactersheetcontrollertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_charactersheetcontroller PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Gui PRIVATE charactersheet core instantmessaging common_core common_widget common_qml rmindmap utils helper network)
target_include_directories(tst_charactersheetcontroller PUBLIC ../helper)
add_test(NAME tst_charactersheetcontroller COMMAND tst_charactersheetcontroller)

add_executable(tst_vectorialmapcontroller tst_vectorialmapcontrollertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_vectorialmapcontroller PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Gui PRIVATE rmindmap core instantmessaging common_core common_widget common_qml network helper utils helper)
target_include_directories(tst_vectorialmapcontroller  PUBLIC ../helper ../../libraries/mindmap/include)
add_test(NAME tst_vectorialmapcontroller COMMAND tst_vectorialmapcontroller)

add_executable(tst_profilecontroller tst_profilecontroller.cpp ../assets/resources.qrc)
target_link_libraries(tst_profilecontroller PUBLIC Qt6::Core Qt6::Test Qt6::StateMachine PRIVATE core instantmessaging common_core common_widget common_qml helper network diceparser_qobject diceparser_shared utils helper dice3d)
target_include_directories(tst_profilecontroller  PUBLIC ../helper )
add_test(NAME tst_profilecontroller COMMAND tst_profilecontroller)

add_executable(tst_mindmapsidemenucontroller tst_mindmapsidemenucontroller.cpp ../assets/resources.qrc)
target_link_libraries(tst_mindmapsidemenucontroller PUBLIC Qt6::Core Qt6::Test Qt6::StateMachine PRIVATE core instantmessaging rmindmap common_core common_widget common_qml helper network diceparser_qobject diceparser_shared utils helper)
target_include_directories(tst_mindmapsidemenucontroller  PUBLIC ../helper )
add_test(NAME tst_mindmapsidemenucontroller COMMAND tst_mindmapsidemenucontroller)

add_executable(tst_sharednotecontroller tst_sharednotecontroller.cpp ../assets/resources.qrc)
target_link_libraries(tst_sharednotecontroller PUBLIC Qt6::Core Qt6::Test Qt6::StateMachine PRIVATE core instantmessaging rmindmap common_core common_widget common_qml helper network diceparser_qobject diceparser_shared utils helper)
target_include_directories(tst_sharednotecontroller  PUBLIC ../helper )
add_test(NAME tst_sharednotecontroller COMMAND tst_sharednotecontroller)


add_executable(tst_theme tst_theme.cpp ../assets/resources.qrc)
target_link_libraries(tst_theme PUBLIC Qt6::Core Qt6::Test Qt6::StateMachine PRIVATE core instantmessaging rmindmap common_core common_widget common_qml helper network diceparser_qobject diceparser_shared utils helper)
target_include_directories(tst_theme  PUBLIC ../helper )
add_test(NAME tst_theme COMMAND tst_theme)


add_executable(tst_remotelog tst_remotelog.cpp ../assets/resources.qrc)
target_link_libraries(tst_remotelog PUBLIC Qt6::Core Qt6::Test Qt6::StateMachine PRIVATE core instantmessaging rmindmap common_core common_widget common_qml helper network diceparser_qobject diceparser_shared utils helper)
target_include_directories(tst_remotelog  PUBLIC ../helper )
add_test(NAME tst_remotelog COMMAND tst_remotelog)
