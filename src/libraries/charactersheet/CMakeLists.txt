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

set(formula_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src/formula)

set(formula_src
  # formula
  ${formula_src_dir}/formulamanager.cpp
  ${formula_src_dir}/parsingtoolformula.cpp
  ${formula_src_dir}/nodes/formulanode.cpp
  ${formula_src_dir}/nodes/operator.cpp
  ${formula_src_dir}/nodes/parenthesesfnode.cpp
  ${formula_src_dir}/nodes/scalaroperatorfnode.cpp
  ${formula_src_dir}/nodes/startnode.cpp
  ${formula_src_dir}/nodes/valuefnode.cpp)

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
set(sheet_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src/charactersheet)

SET(character_sources
    ${sheet_src_dir}/charactersheet.cpp
    ${sheet_src_dir}/charactersheetitem.cpp
    ${sheet_src_dir}/charactersheetmodel.cpp
    ${sheet_src_dir}/controllers/fieldcontroller.cpp
    ${sheet_src_dir}/controllers/sheetcontroller.cpp
    ${sheet_src_dir}/controllers/tablefield.cpp
    ${sheet_src_dir}/controllers/tablemodel.cpp
    ${sheet_src_dir}/controllers/section.cpp
    ${sheet_src_dir}/controllers/slidercontroller.cpp
    ${sheet_src_dir}/csitem.cpp
    ${sheet_src_dir}/imagemodel.cpp
    ${sheet_src_dir}/rolisteamimageprovider.cpp
    ${sheet_src_dir}/worker/ioworker.cpp
    ${sheet_src_dir}/fieldfactory.cpp
)



SET(character_headers
  ${inc_sheet}/charactersheet_global.h
  ${inc_sheet}/charactersheetitem.h
  ${inc_sheet}/charactersheetmodel.h
  ${inc_sheet}/controllers/fieldcontroller.h
  ${inc_sheet}/controllers/section.h
  ${inc_sheet}/controllers/tablefield.h
  ${inc_sheet}/controllers/tablemodel.h
  ${inc_sheet}/controllers/slidercontroller.h
  ${inc_sheet}/charactersheet.h
  ${inc_sheet}/imagemodel.h
  ${inc_sheet}/rolisteamimageprovider.h
  ${inc_sheet}/csitem.h
  ${inc_sheet}/worker/ioworker.h
  ${inc_sheet}/controllers/sheetcontroller.h
  ${inc_sheet}/abstractapplicationcontroller.h
  ${inc_sheet}/fieldfactory.h
)



set(qml_sources ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/CheckBoxField.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/CheckBoxFieldv1.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/DiceButton.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/DiceButtonv1.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/ImageField.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/ImageFieldv1.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/example.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/PageButton.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/RLabel.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/SelectField.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/SelectFieldv1.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/Table.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/TextAreaField.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/TextAreaFieldv1.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/TextFieldField.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/TextFieldFieldv1.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/TextInputField.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/TextInputFieldv1.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/WebView.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/SliderField.qml
)

set(documentation Readme.md)
set(charactersheet_QRC ${CMAKE_CURRENT_SOURCE_DIR}/qml/charactersheet.qrc)



#qt_add_qml_module(charactersheet
#    URI Rolisteam
#    VERSION 1.1
#    SOURCES
#      ${qml_sources}
#)
#message(${character_sources})
#message(${character_headers})
#message(${qml_sources})
#message(${documentation})
#message(${charactersheet_QRC})

add_library(charactersheet SHARED ${character_sources} ${character_headers} ${qml_sources}  ${documentation} ${charactersheet_QRC})

qt_add_qml_module(charactersheet
    URI charactersheet
    VERSION 1.0
    SOURCES
      ${sheet_src_dir}/controllers/sheetcontroller.cpp
      ${inc_sheet}/controllers/sheetcontroller.h
      ${inc_sheet}/abstractapplicationcontroller.h
)

target_link_libraries(charactersheet PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Qml Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::QuickWidgets Qt${QT_VERSION_MAJOR}::WebEngineWidgets Qt${QT_VERSION_MAJOR}::WebEngineCore PRIVATE charactersheet_formula utils)
set_target_properties(charactersheet PROPERTIES PUBLIC_HEADER "${character_headers}")
target_compile_definitions(charactersheet PRIVATE CHARACTERSHEET_LIBRARY)
target_include_directories(charactersheet
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
      ${CMAKE_CURRENT_SOURCE_DIR}/src
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/charactersheet>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/charactersheet/controllers>
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

#===========================================================

set(inc_sheet ${CMAKE_CURRENT_SOURCE_DIR}/include/charactersheet_widgets)
set(widget_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src/widget)

set(header_widget ${inc_sheet}/charactersheet_widget_global.h
${inc_sheet}/sheetwidget.h)

set(src_widget 
    ${widget_src_dir}/sheetwidget.cpp
)



add_library(charactersheet_widgets SHARED ${src_widget} ${header_widget} ${charactersheet_QRC})
target_link_libraries(charactersheet_widgets PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Qml Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::QuickWidgets Qt${QT_VERSION_MAJOR}::WebEngineWidgets Qt${QT_VERSION_MAJOR}::WebEngineCore PRIVATE charactersheet)
set_target_properties(charactersheet_widgets PROPERTIES PUBLIC_HEADER "${header_widget}")
target_compile_definitions(charactersheet_widgets PRIVATE CHARACTERSHEET_WIDGET_LIBRARY)
target_include_directories(charactersheet_widgets PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

include(install_helper)
rinstallLib(charactersheet_formula libraries)
rinstallLib(charactersheet_widgets libraries)
rinstallLib(charactersheet libraries)
