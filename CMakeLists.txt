cmake_minimum_required(VERSION 3.10)
project(rcse)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(FindPoppler)

add_definitions(-DVERSION_MAJOR=1 -DVERSION_MIDDLE=10 -DVERSION_MINOR=0)
set(QT_REQUIRED_VERSION "5.12.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets Gui Quick QuickWidgets WebEngineWidgets PrintSupport WebEngine)

if (TESTS)
add_subdirectory(tests)
endif()


set(documentation README.md)

set(ressources qmlfile.qrc rcse_resources.qrc)


set(sources  src/main.cpp
    src/controllers/charactercontroller.cpp
    src/controllers/editorcontroller.cpp
    src/controllers/imagecontroller.cpp
    src/controllers/qmlgeneratorcontroller.cpp
    src/data/mockcharacter.cpp
    src/mainwindow.cpp
    src/canvas.cpp
    src/fieldmodel.cpp
    src/borderlisteditor.cpp
    src/qmlhighlighter.cpp
    src/delegate/alignmentdelegate.cpp
    src/codeeditor.cpp
    src/delegate/typedelegate.cpp
    src/canvasfield.cpp
    src/dialog/aboutrcse.cpp
    src/dialog/pdfmanager.cpp
    src/delegate/fontdelegate.cpp
    src/preferences/preferencesdialog.cpp
    src/preferences/preferencesmanager.cpp
    src/dialog/sheetproperties.cpp
    src/itemeditor.cpp
    src/tablecanvasfield.cpp
    src/dialog/columndefinitiondialog.cpp
    src/undo/addfieldcommand.cpp
    src/undo/deletefieldcommand.cpp
    src/undo/movefieldcommand.cpp
    src/undo/setfieldproperties.cpp
    src/undo/addpagecommand.cpp
    src/undo/deletepagecommand.cpp
    src/undo/setbackgroundimage.cpp
    src/undo/addcharactercommand.cpp
    src/undo/deletecharactercommand.cpp
    src/undo/setpropertyonallcharacters.cpp
    src/widgets/codeedit.cpp
    src/delegate/pagedelegate.cpp
    src/dialog/codeeditordialog.cpp
    src/widgets/fieldview.cpp
    src/common/widgets/logpanel.cpp
    src/common/controller/logcontroller.cpp
    src/qmltypesregister.cpp
  src/qmlgeneratorvisitor.cpp)

set(headers src/mainwindow.h
    src/canvas.h
    src/controllers/charactercontroller.h
    src/controllers/editorcontroller.h
    src/controllers/imagecontroller.h
    src/controllers/qmlgeneratorcontroller.h
    src/data/mockcharacter.h
    src/fieldmodel.h
    src/borderlisteditor.h
    src/qmlhighlighter.h
    src/delegate/alignmentdelegate.h
    src/codeeditor.h
    src/delegate/typedelegate.h
    src/canvasfield.h
    src/dialog/aboutrcse.h
    src/dialog/pdfmanager.h
    src/delegate/fontdelegate.h
    src/preferences/preferencesdialog.h
    src/preferences/preferencesmanager.h
    src/dialog/sheetproperties.h
    src/itemeditor.h
    src/tablecanvasfield.h
    src/dialog/columndefinitiondialog.h
    src/undo/addfieldcommand.h
    src/undo/deletefieldcommand.h
    src/undo/movefieldcommand.h
    src/undo/setfieldproperties.h
    src/undo/addpagecommand.h
    src/undo/deletepagecommand.h
    src/undo/setbackgroundimage.h
    src/undo/addcharactercommand.h
    src/undo/deletecharactercommand.h
    src/undo/setpropertyonallcharacters.h
    src/widgets/codeedit.h
    src/delegate/pagedelegate.h
    src/dialog/codeeditordialog.h
    src/widgets/fieldview.h
    src/common/widgets/logpanel.h
    src/common/controller/logcontroller.h
    src/qmltypesregister.h
    src/qmlgeneratorvisitor.h)


set(UI_files   src/mainwindow.ui
        src/dialog/aboutrcse.ui
        src/dialog/pdfmanager.ui
        src/preferences/preferencesdialog.ui
        src/dialog/sheetproperties.ui
        src/dialog/columndefinitiondialog.ui
        src/widgets/codeedit.ui
        src/dialog/codeeditordialog.ui
      src/common/widgets/logpanel.ui)

add_subdirectory(src/charactersheet)
add_subdirectory(src/diceparser)

find_package(Poppler COMPONENTS Qt5)
if(POPPLER_FOUND)
add_definitions(-DWITH_PDF)
endif()

add_executable(rcse ${sources} ${headers} ${UI_files} ${ressources})
if(POPPLER_FOUND)
    target_link_libraries(rcse PUBLIC Qt5::Widgets Qt5::Gui Qt5::Qml Qt5::Quick Qt5::QuickWidgets Qt5::PrintSupport Qt5::WebEngineWidgets Poppler::Qt5 Qt5::WebEngine charactersheet diceparser_shared)
    target_compile_definitions(rcse PRIVATE WITH_PDF=1 RCSE=1)
else()
    target_link_librarues(rcse PUBLIC Qt5::Widgets Qt5::Gui Qt5::Qml Qt5::Quick Qt5::QuickWidgets Qt5::PrintSupport Qt5::WebEngineWidgets charactersheet diceparser_shared)
    target_compile_definitions(rcse PRIVATE RCSE=1)
endif()

target_include_directories(rcse PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/charactersheet
    ${CMAKE_CURRENT_SOURCE_DIR}/src/preferences
    ${CMAKE_CURRENT_SOURCE_DIR}/src/diceparser)
