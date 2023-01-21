cmake_minimum_required(VERSION 3.16)
#project(rcse)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core LinguistTools Widgets Gui Concurrent Quick Pdf QuickWidgets PdfWidgets WebEngineWidgets PrintSupport WebEngineCore)


set(documentation README.md)

#${CMAKE_SOURCE_DIR}/resources/rcse/qmlfile.qrc

set(ressources  ${CMAKE_SOURCE_DIR}/resources/rcse/rcse_resources.qrc)


set(sources  main.cpp
    controllers/charactercontroller.cpp
    controllers/editorcontroller.cpp
    controllers/imagecontroller.cpp
    controllers/qmlgeneratorcontroller.cpp
    controllers/rcseapplicationcontroller.cpp
    controllers/maincontroller.cpp
    data/mockcharacter.cpp
    mainwindow.cpp
    canvas.cpp
    fieldmodel.cpp
    borderlisteditor.cpp
    qmlhighlighter.cpp
    delegate/alignmentdelegate.cpp
    codeeditor.cpp
    delegate/typedelegate.cpp
    canvasfield.cpp
    dialog/aboutrcse.cpp
    dialog/pdfmanager.cpp
    delegate/fontdelegate.cpp
    preferences/preferencesdialog.cpp
    preferences/preferencesmanager.cpp
    dialog/sheetproperties.cpp
    itemeditor.cpp
    tablecanvasfield.cpp
    dialog/columndefinitiondialog.cpp
    data/characterlist.cpp
    undo/addfieldcommand.cpp
    undo/deletefieldcommand.cpp
    undo/movefieldcommand.cpp
    undo/setfieldproperties.cpp
    undo/addpagecommand.cpp
    undo/deletepagecommand.cpp
    undo/setbackgroundimage.cpp
    undo/addcharactercommand.cpp
    undo/deletecharactercommand.cpp
    undo/setpropertyonallcharacters.cpp
    widgets/codeedit.cpp
    delegate/pagedelegate.cpp
    dialog/codeeditordialog.cpp
    widgets/fieldview.cpp
    qmltypesregister.cpp
  qmlgeneratorvisitor.cpp serializerhelper.cpp)

set(headers mainwindow.h
    canvas.h
    controllers/charactercontroller.h
    controllers/editorcontroller.h
    controllers/imagecontroller.h
    controllers/qmlgeneratorcontroller.h
    controllers/maincontroller.h
    controllers/rcseapplicationcontroller.h
    data/characterlist.h
    data/mockcharacter.h
    fieldmodel.h
    borderlisteditor.h
    qmlhighlighter.h
    delegate/alignmentdelegate.h
    codeeditor.h
    delegate/typedelegate.h
    canvasfield.h
    dialog/aboutrcse.h
    dialog/pdfmanager.h
    delegate/fontdelegate.h
    preferences/preferencesdialog.h
    preferences/preferencesmanager.h
    dialog/sheetproperties.h
    itemeditor.h
    tablecanvasfield.h
    dialog/columndefinitiondialog.h
    undo/addfieldcommand.h
    undo/deletefieldcommand.h
    undo/movefieldcommand.h
    undo/setfieldproperties.h
    undo/addpagecommand.h
    undo/deletepagecommand.h
    undo/setbackgroundimage.h
    undo/addcharactercommand.h
    undo/deletecharactercommand.h
    undo/setpropertyonallcharacters.h
    widgets/codeedit.h
    delegate/pagedelegate.h
    dialog/codeeditordialog.h
    widgets/fieldview.h
    qmltypesregister.h
    qmlgeneratorvisitor.h
  serializerhelper.h  )


set(UI_files   mainwindow_rcse.ui
        dialog/aboutrcse.ui
        dialog/pdfmanager.ui
        preferences/preferencesdialog.ui
        dialog/sheetproperties.ui
        dialog/columndefinitiondialog.ui
        widgets/codeedit.ui
        dialog/codeeditordialog.ui)





add_executable(rcse ${sources} ${headers} ${UI_files} ${ressources})

target_link_libraries(rcse
  PUBLIC
    Qt6::Widgets
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickWidgets
    Qt6::PrintSupport
    Qt6::WebEngineWidgets
    Qt6::Pdf
    Qt6::PdfWidgets
    Qt6::WebEngineCore
    Qt6::Concurrent
  PRIVATE
    charactersheet
    charactersheet_formula
    diceparser_shared
    diceparser_qobject
    common_core
    common_widget
    utils
    )


target_compile_definitions(rcse PRIVATE RCSE=1)

target_include_directories(rcse PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/preferences
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/libraries/charactersheet
    ${CMAKE_SOURCE_DIR}/src/libraries
    ${CMAKE_BINARY_DIR})


include(install_helper)
rinstallRT(rcse rcseComp)
