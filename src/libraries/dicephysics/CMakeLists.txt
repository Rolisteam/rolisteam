cmake_minimum_required(VERSION 3.14)

project(dice3d VERSION 1.10.0 DESCRIPTION "Dice in 3D with Physics")

set(CMAKE_AUTOMOC ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_SUPPORT_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Quick3D Quick3DPhysics Multimedia Svg)

set(headers controllers/dice3dcontroller.h controllers/dicecontroller.h data/dicemodel.h data/dicehelper.h controllers/dice3d_global.h)

add_library(${PROJECT_NAME}
    SHARED
        controllers/dice3dcontroller.cpp
        data/dicemodel.cpp
        controllers/dicecontroller.cpp
        ${headers}
)
target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Quick
    Qt::Quick3D
    Qt::Quick3DPhysics
    Qt::Multimedia
    Qt::Svg
)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${headers}")
target_compile_definitions(${PROJECT_NAME} PRIVATE DICE3D_LIBRARY)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/data
    ${CMAKE_CURRENT_SOURCE_DIR}/controllers
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/data>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/controllers>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)


# Resources
set(ASSETS
    #assets/maps/cloth-heightmap.png
    #assets/maps/numbers-normal.png
    #assets/maps/numbers.png
    #assets/maps/weave.png
    #assets/maps/black-heightmap.png
    assets/maps/d4_Base_color.png
    assets/maps/d4_Normal_OpenGL.png
    assets/maps/d6_Base_color.png
    assets/maps/d6_Normal_OpenGL.png
    assets/maps/d8_Base_color.png
    assets/maps/d8_Normal_OpenGL.png
    assets/maps/d10_Base_color.png
    assets/maps/d10_Normal_OpenGL.png
    assets/maps/d12_Base_color.png
    assets/maps/d12_Normal_OpenGL.png
    assets/maps/d20_Base_color.png
    assets/maps/d20_Normal_OpenGL.png
    assets/maps/d100_Base_color.png
    assets/maps/d100_Normal_OpenGL.png
    assets/meshes/d4.mesh
    assets/meshes/d6.mesh
    assets/meshes/d8.mesh
    assets/meshes/d10.mesh
    assets/meshes/d12.mesh
    assets/meshes/d20.mesh
    assets/meshes/d100.mesh
    assets/icons/menuIcon.svg
    assets/icons/dice3d.svg
    assets/sounds/onedice.wav
    assets/sounds/rolling.wav
)

set_source_files_properties(qml/Main.qml PROPERTIES
    QT_RESOURCE_ALIAS Main.qml
)
set_source_files_properties(qml/DiceInterface.qml PROPERTIES
QT_RESOURCE_ALIAS DiceInterface.qml
)
set_source_files_properties(qml/RegularDice.qml PROPERTIES
    QT_RESOURCE_ALIAS RegularDice.qml
)
set_source_files_properties(qml/SideMenu.qml PROPERTIES
    QT_RESOURCE_ALIAS SideMenu.qml
)

qt6_add_qml_module(${PROJECT_NAME}
  URI DicePhysics
  VERSION 1.0
  RESOURCE_PREFIX
    "/DicePhysics"
  QML_FILES
    qml/Main.qml
    qml/DiceInterface.qml
    qml/RegularDice.qml
    qml/SideMenu.qml
  SOURCES
    data/dicemodel.h
    data/dicemodel.cpp
    controllers/dicecontroller.h
    controllers/dicecontroller.cpp
  VERSION 1.0 
)

qt_add_resources(${PROJECT_NAME} "qml"
    PREFIX
        "/dice3d/"
    BASE
        "assets"
    FILES
        ${ASSETS}
)


include(install_helper)
rinstallLib(${PROJECT_NAME} libraries)
