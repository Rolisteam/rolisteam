cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Widgets Pdf)

add_executable(manual_pdf tst_pdf.cpp ${TEST_ROOT_DIR}/auto/assets/resources.qrc)
target_link_libraries(manual_pdf PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Pdf PRIVATE core rwidgets common_core common_widget rmindmap common_qml diceparser_qobject diceparser_shared utils qml_views qml_components network )

