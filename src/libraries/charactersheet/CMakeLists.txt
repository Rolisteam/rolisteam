cmake_minimum_required(VERSION 3.16)

project(charactersheet VERSION 1.10.0 DESCRIPTION "Manage charactersheet")
option(CHARACTERSHEET_TESTS "enable tests compilation" OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(QT_REQUIRED_VERSION "6.3.0")
set(QT_VERSION_MAJOR "6")
find_package(Qt${QT_VERSION_MAJOR} ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Test Gui Svg Qml Quick QuickWidgets WebEngineWidgets WebEngineCore)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} result node)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(src_dir ${CMAKE_CURRENT_SOURCE_DIR})


set(formula_src
  # formula
  ${src_dir}/formula/formulamanager.cpp
  ${src_dir}/formula/parsingtoolformula.cpp
  ${src_dir}/formula/nodes/formulanode.cpp
  ${src_dir}/formula/nodes/operator.cpp
  ${src_dir}/formula/nodes/parenthesesfnode.cpp
  ${src_dir}/formula/nodes/scalaroperatorfnode.cpp
  ${src_dir}/formula/nodes/startnode.cpp
  ${src_dir}/formula/nodes/valuefnode.cpp)

set(inc ${CMAKE_CURRENT_SOURCE_DIR}/include/charactersheet_formula)
set(formula_header
  # formula
  ${inc}/formula_global.h
  ${inc}/formulamanager.h
  ${inc}/parsingtoolformula.h
  ${inc}/nodes/formulanode.h
  ${inc}/nodes/operator.h
  ${inc}/nodes/parenthesesfnode.h
  ${inc}/nodes/scalaroperatorfnode.h
  ${inc}/nodes/startnode.h
  ${inc}/nodes/valuefnode.h
  )

add_library(charactersheet_formula SHARED ${formula_src} ${formula_header})
target_link_libraries(charactersheet_formula PUBLIC Qt${QT_VERSION_MAJOR}::Core)
set_target_properties(charactersheet_formula PROPERTIES PUBLIC_HEADER "${formula_header}")
target_compile_definitions(charactersheet_formula PRIVATE CHARACTERSHEET_FORMULA_LIBRARY)
target_include_directories(charactersheet_formula PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/formula
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
#===========================================================

set(inc_sheet ${CMAKE_CURRENT_SOURCE_DIR}/include/charactersheet)

SET(character_sources
    ${src_dir}/charactersheet.cpp
    ${src_dir}/charactersheetitem.cpp
    ${src_dir}/charactersheetmodel.cpp
    ${src_dir}/csitem.cpp
    ${src_dir}/field.cpp
    ${src_dir}/imagemodel.cpp
    ${src_dir}/rolisteamimageprovider.cpp
    ${src_dir}/section.cpp
    ${src_dir}/tablefield.cpp
)

SET(character_headers
  ${inc_sheet}/charactersheet_global.h
  ${inc_sheet}/charactersheetitem.h
  ${inc_sheet}/charactersheetmodel.h
  ${inc_sheet}/charactersheet.h
  ${inc_sheet}/imagemodel.h
  ${inc_sheet}/rolisteamimageprovider.h
    ${inc_sheet}/csitem.h
    ${inc_sheet}/field.h
    ${inc_sheet}/section.h
    ${inc_sheet}/tablefield.h
)



set(qml_sources ${src_dir}/qml/Rolisteam/CheckBoxField.qml
  ${src_dir}/qml/Rolisteam/CheckBoxFieldv1.qml
  ${src_dir}/qml/Rolisteam/DiceButton.qml
  ${src_dir}/qml/Rolisteam/DiceButtonv1.qml
  ${src_dir}/qml/Rolisteam/ImageField.qml
  ${src_dir}/qml/Rolisteam/ImageFieldv1.qml
  ${src_dir}/qml/Rolisteam/main.qml
  ${src_dir}/qml/Rolisteam/PageButton.qml
  ${src_dir}/qml/Rolisteam/RLabel.qml
  ${src_dir}/qml/Rolisteam/SelectField.qml
  ${src_dir}/qml/Rolisteam/SelectFieldv1.qml
  ${src_dir}/qml/Rolisteam/Table.qml
  ${src_dir}/qml/Rolisteam/TextAreaField.qml
  ${src_dir}/qml/Rolisteam/TextAreaFieldv1.qml
  ${src_dir}/qml/Rolisteam/TextFieldField.qml
  ${src_dir}/qml/Rolisteam/TextFieldFieldv1.qml
  ${src_dir}/qml/Rolisteam/TextInputField.qml
  ${src_dir}/qml/Rolisteam/TextInputFieldv1.qml
  ${src_dir}/qml/Rolisteam/WebView.qml
)

set(documentation Readme.md)
set(charactersheet_QRC ${src_dir}/qml/charactersheet.qrc)


add_library(charactersheet SHARED ${character_sources} ${character_headers} ${qml_sources}  ${documentation} ${charactersheet_QRC})
target_link_libraries(charactersheet PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Qml Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::QuickWidgets Qt${QT_VERSION_MAJOR}::WebEngineWidgets Qt${QT_VERSION_MAJOR}::WebEngineCore PRIVATE charactersheet_formula)
set_target_properties(charactersheet PROPERTIES PUBLIC_HEADER "${character_headers}")
target_compile_definitions(charactersheet PRIVATE CHARACTERSHEET_LIBRARY)
target_include_directories(charactersheet PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

#===========================================================

set(inc_sheet ${CMAKE_CURRENT_SOURCE_DIR}/include/charactersheet_widgets)

set(header_widget ${inc_sheet}/charactersheet_widget_global.h
${inc_sheet}/sheetwidget.h)

set(src_widget 
    ${src_dir}/widget/sheetwidget.cpp
)



add_library(charactersheet_widgets SHARED ${src_widget} ${header_widget} ${charactersheet_QRC})
target_link_libraries(charactersheet_widgets PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Qml Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::QuickWidgets Qt${QT_VERSION_MAJOR}::WebEngineWidgets Qt${QT_VERSION_MAJOR}::WebEngineCore PRIVATE charactersheet)
set_target_properties(charactersheet_widgets PROPERTIES PUBLIC_HEADER "${header_widget}")
target_compile_definitions(charactersheet_widgets PRIVATE CHARACTERSHEET_WIDGET_LIBRARY)
target_include_directories(charactersheet_widgets PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)



#if (BUILD_TESTING)
#enable_testing(true)
#add_subdirectory(test)
#endif()


#include(GNUInstallDirs)

#install(TARGETS charactersheet charactersheet_widgets charactersheet_formula
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(install_helper)
rinstallLib(charactersheet_formula charactersheet_widgets charactersheet rolisteam)
