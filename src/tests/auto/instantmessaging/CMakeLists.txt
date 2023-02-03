cmake_minimum_required(VERSION 3.16)

include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Widgets Qml Quick)

set(RESOURCE_FILES ../../../../rolisteam.qrc)

add_executable(tst_im tst_im.cpp ${RESOURCE_FILES} ${TEST_ROOT_DIR}/auto/assets/resources.qrc)
target_link_libraries(tst_im
  PUBLIC
    Qt6::Core
    Qt6::Test
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
  PRIVATE
    core
    instantmessaging
    rwidgets
    common_core
    helper
    common_widget
    common_qml
    qml_views
    network
    diceparser_qobject
    diceparser_shared
    charactersheet_widgets
    qml_components
  )
target_include_directories(tst_im  PUBLIC ../helper )
add_test(NAME tst_im COMMAND tst_im)
