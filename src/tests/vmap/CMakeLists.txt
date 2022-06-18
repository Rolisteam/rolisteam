cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Widgets)

add_executable(tst_vmap tst_vmap.cpp)
target_link_libraries(tst_vmap PUBLIC Qt6::Core Qt6::Test Qt6::Widgets PRIVATE core rwidgets instantmessaging common_core common_widget common_qml )
add_test(NAME tst_vmap COMMAND tst_vmap)


add_executable(tst_vmapitemcontroller tst_vmapitemcontroller.cpp)
target_link_libraries(tst_vmapitemcontroller PUBLIC Qt6::Core Qt6::Test Qt6::Widgets PRIVATE core rwidgets instantmessaging common_core common_widget common_qml helper network)
target_include_directories(tst_vmapitemcontroller PUBLIC ../helper)
add_test(NAME tst_vmapitemcontroller COMMAND tst_vmapitemcontroller)


add_executable(manual_vmap_items tst_vmap_with_item.cpp ../assets/resources.qrc)
target_link_libraries(manual_vmap_items PUBLIC Qt6::Core Qt6::Test Qt6::Widgets PRIVATE core rwidgets common_core common_widget common_qml utils qml_views qml_components network)
