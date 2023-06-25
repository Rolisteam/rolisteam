cmake_minimum_required(VERSION 3.16)

project(rolisteam)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    mainwindow.cpp
    rolisteamapplication.cpp
    )


set(HEADERS
    mainwindow.h
    rolisteamapplication.h
    )



set(UIS mainwindow.ui)


find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets Gui Quick3D LinguistTools Scxml MultimediaWidgets Multimedia)

set(RESOURCE_FILES ${CMAKE_SOURCE_DIR}/rolisteam.qrc)



if("${CAMKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/rolisteam_autogen/mocs_compilation.cpp" PROPERTIES COMPILE_FLAGS "-Wno-undefined-reinterpret-cast -Wno-useless-cast")
endif()
add_executable(rolisteam ${SOURCES} ${HEADERS} ${UIS} ${RESOURCE_FILES})
qt6_add_statecharts(rolisteam applicationstate.scxml)
target_include_directories(rolisteam PRIVATE ../../libraries ${CMAKE_BINARY_DIR})


target_link_libraries(rolisteam PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::MultimediaWidgets Qt6::Multimedia Qt6::Scxml Qt6::Quick3D PRIVATE core rwidgets common_core common_widget common_qml instantmessaging charactersheet_widgets diceparser_qobject utils rmindmap qml_views qml_components network)

include(install_helper)
rinstallRT(rolisteam rolisteamComp)
