cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_upnp tst_upnp.cpp ../assets/resources.qrc)
target_link_libraries(tst_upnp PUBLIC Qt6::Core Qt6::Test PRIVATE core instantmessaging common_core common_widget common_qml network)
add_test(NAME tst_upnp COMMAND tst_upnp)

add_executable(tst_ipchecker tst_ipchecker.cpp ../assets/resources.qrc)
target_link_libraries(tst_ipchecker PUBLIC Qt6::Core Qt6::Test PRIVATE core instantmessaging common_core common_widget common_qml network helper utils)
target_include_directories(tst_ipchecker PUBLIC ../helper)
add_test(NAME tst_ipchecker COMMAND tst_ipchecker)

add_executable(tst_tipchecker tst_tipchecker.cpp ../assets/resources.qrc)
target_link_libraries(tst_tipchecker PUBLIC Qt6::Core Qt6::Test PRIVATE core instantmessaging common_core common_widget common_qml network helper utils)
target_include_directories(tst_tipchecker PUBLIC ../helper)
add_test(NAME tst_tipchecker COMMAND tst_tipchecker)

add_executable(tst_update tst_update.cpp ../assets/resources.qrc)
target_link_libraries(tst_update PUBLIC Qt6::Core Qt6::Test PRIVATE core instantmessaging common_core common_widget common_qml network helper utils)
target_include_directories(tst_update PUBLIC ../helper)
add_test(NAME tst_update COMMAND tst_update)

