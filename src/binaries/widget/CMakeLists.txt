cmake_minimum_required(VERSION 3.10)

project(rolisteam)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

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



set(QT_REQUIRED_VERSION "5.15.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets Gui LinguistTools Scxml MultimediaWidgets Multimedia)

set(RESOURCE_FILES ${CMAKE_SOURCE_DIR}/rolisteam.qrc)

qt5_add_statecharts(QT_SCXML_COMPILED applicationstate.scxml)
set_property(SOURCE ${QT_SCXML_COMPILED} PROPERTY SKIP_AUTOMOC ON)

set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/rolisteam_autogen/mocs_compilation.cpp" PROPERTIES COMPILE_FLAGS "-Wno-undefined-reinterpret-cast -Wno-useless-cast")
add_executable(rolisteam ${SOURCES} ${HEADERS} ${UIS} ${RESOURCE_FILES} ${QT_SCXML_COMPILED})
target_include_directories(rolisteam PRIVATE ../../libraries)

if(${HAS_PDF})
    message("Client with PDF")
    add_definitions(-DWITH_PDF=1)
endif()

target_link_libraries(rolisteam PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::MultimediaWidgets Qt5::Multimedia Qt5::Scxml PRIVATE core rwidgets common_core common_widget common_qml instantmessaging charactersheet_widgets diceparser_qobject qml_views)

install(TARGETS rolisteam
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)
