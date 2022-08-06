add_subdirectory(fuzzer)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Widgets)

add_executable(manual_sheet tst_sheet.cpp ${TEST_ROOT_DIR}/auto/assets/resources.qrc)
target_link_libraries(manual_sheet PUBLIC Qt6::Core Qt6::Test Qt6::Widgets PRIVATE core rwidgets rmindmap common_core common_widget common_qml diceparser_qobject diceparser_shared utils qml_views qml_components instantmessaging network charactersheet charactersheet_widgets charactersheet_formula )

add_executable(manual_image_model tst_imagemodel.cpp ${TEST_ROOT_DIR}/auto/assets/resources.qrc)
target_link_libraries(manual_image_model PUBLIC Qt6::Core Qt6::Test Qt6::Widgets PRIVATE core rwidgets rmindmap common_core common_widget common_qml diceparser_qobject diceparser_shared utils qml_views qml_components instantmessaging network charactersheet charactersheet_widgets charactersheet_formula )
