cmake_minimum_required(VERSION 3.18)

project(charactersheet VERSION 1.10.0 DESCRIPTION "Manage charactersheet")
option(CHARACTERSHEET_TESTS "enable tests compilation" OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(QT_REQUIRED_VERSION "5.14.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Test Gui Svg Qml Quick)


include_directories(${CMAKE_CURRENT_SOURCE_DIR} result node)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

SET(character_sources
    #${CMAKE_CURRENT_SOURCE_DIR}/charactersheetbutton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/charactersheet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/charactersheetitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/charactersheetmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/csitem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/field.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imagemodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qqmlhelpers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qqmlobjectlistmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rolisteamimageprovider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/section.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/sheetwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tablefield.cpp
    // formula
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/formulamanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/parsingtoolformula.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/nodes/formulanode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/nodes/operator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/nodes/parenthesesfnode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/nodes/scalaroperatorfnode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/nodes/startnode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/nodes/valuefnode.cpp
)

SET(character_headers
    #${CMAKE_CURRENT_SOURCE_DIR}/charactersheetbutton.h
    ${CMAKE_CURRENT_SOURCE_DIR}/charactersheet.h
    ${CMAKE_CURRENT_SOURCE_DIR}/charactersheetitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/charactersheetmodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/csitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/field.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imagemodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qqmlhelpers.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qqmlobjectlistmodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/rolisteamimageprovider.h
    ${CMAKE_CURRENT_SOURCE_DIR}/section.h
   # ${CMAKE_CURRENT_SOURCE_DIR}/sheetwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tablefield.h
    # formula
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/formulamanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/parsingtoolformula.h
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/nodes/formulanode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/nodes/operator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/nodes/parenthesesfnode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/nodes/scalaroperatorfnode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/nodes/startnode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/nodes/valuefnode.h
)

if (TESTS)
enable_testing(true)
add_subdirectory(test)
endif()

set(qml_sources ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/CheckBoxField.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/CheckBoxFieldv1.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/DiceButton.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/DiceButtonv1.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/ImageField.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/ImageFieldv1.qml
  ${CMAKE_CURRENT_SOURCE_DIR}/qml/Rolisteam/main.qml
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
)

set(documentation Readme.md)

add_library(charactersheet SHARED ${character_sources} ${character_headers} ${qml_sources}  ${documentation})
target_link_libraries(charactersheet PUBLIC Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Qml Qt5::Quick Qt5::QuickWidgets Qt5::WebEngineWidgets Qt5::WebEngine)
