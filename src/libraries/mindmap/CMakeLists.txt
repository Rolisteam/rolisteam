cmake_minimum_required(VERSION 3.16)

project(rmindmap VERSION 1.0.0 DESCRIPTION "Map your mind")
option(BUILD_GUI OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Add a compiler flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")


find_package(Qt6  COMPONENTS Core Svg Quick QuickControls2 REQUIRED)

set(inc_dir include/mindmap)
set(mindmap_header
    ${inc_dir}/mindmap_global.h
    ${inc_dir}/controller/selectioncontroller.h
    ${inc_dir}/controller/spacingcontroller.h
    ${inc_dir}/data/link.h
    ${inc_dir}/data/mindnode.h
    ${inc_dir}/data/nodestyle.h
    ${inc_dir}/data/mindmaptypes.h
    ${inc_dir}/model/linkmodel.h
    ${inc_dir}/model/imagemodel.h
    ${inc_dir}/geometry/linknode.h
    ${inc_dir}/model/nodestylemodel.h
    ${inc_dir}/qmlItems/linkitem.h
    ${inc_dir}/command/addnodecommand.h
    ${inc_dir}/worker/fileserializer.h
    ${inc_dir}/command/reparentingnodecommand.h
    ${inc_dir}/command/dragnodecommand.h
    ${inc_dir}/command/removenodecommand.h
    ${inc_dir}/command/addimagetonodecommand.h
    ${inc_dir}/command/removeimagefromnodecommand.h
    ${inc_dir}/model/boxmodel.h
    ${inc_dir}/model/nodeimageprovider.h)

        # main.cpp
set(mindmap_src
    src/controller/selectioncontroller.cpp
    src/controller/spacingcontroller.cpp
    src/data/link.cpp
    src/data/mindnode.cpp
    src/data/nodestyle.cpp
    src/model/linkmodel.cpp
    src/model/imagemodel.cpp
    src/command/dragnodecommand.cpp
    src/command/addnodecommand.cpp
    src/command/removenodecommand.cpp
    src/command/addimagetonodecommand.cpp
    src/command/removeimagefromnodecommand.cpp
    src/model/nodestylemodel.cpp
    src/worker/fileserializer.cpp
    src/command/reparentingnodecommand.cpp
    src/geometry/linknode.cpp
    src/qmlItems/linkitem.cpp
    src/model/boxmodel.cpp
    src/model/nodeimageprovider.cpp)

set(mindmap_qml
    resources/qml/IconButton.qml
    resources/qml/Link.qml
    resources/qml/MindMap.qml
    resources/qml/MindMenu.qml
    resources/qml/Node.qml
    resources/qml/DrawerMenu.qml
    resources/qml/main.qml
)



set(mindmap_bin_src 
    main.cpp 
    src/controller/mindmapcontroller.cpp
    src/controller/mindmapcontroller.h
    )


add_library(rmindmap SHARED ${mindmap_header} ${mindmap_src} ${mindmap_qml} rmindmap.qrc)
target_link_libraries(rmindmap PUBLIC Qt6::Core Qt6::Svg Qt6::Svg Qt6::Qml Qt6::Quick Qt6::QuickControls2 PRIVATE utils)

set_target_properties(rmindmap PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(rmindmap PROPERTIES PUBLIC_HEADER "${mindmap_header}")
target_compile_definitions(rmindmap PRIVATE MINDMAP_LIBRARY)

include(GNUInstallDirs)

target_include_directories(rmindmap
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mindmap/data
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mindmap/command
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mindmap/controller
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mindmap/qmlItems
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mindmap/geometry
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mindmap/worker
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mindmap/model
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
#if(WIN32)
#    set_target_properties(rmindmap PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
#endif()

#install(TARGETS rmindmap
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#   PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
include(install_helper)
rinstallLib(rmindmap rolisteam)

IF(BUILD_GUI)
    add_executable(rmindmap-gui ${mindmap_bin_src})
    target_link_libraries(rmindmap-gui PUBLIC rmindmap)
    target_include_directories(rmindmap-gui PRIVATE src)

    #install(TARGETS rmindmap-gui
    #    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    #   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    #   PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    rinstallRT(rmindmap-gui mindmap)
ENDIF()





