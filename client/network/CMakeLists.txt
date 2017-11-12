cmake_minimum_required(VERSION 2.8)

project(rolisteamd)


option(UPDATE_TRANSLATIONS "update Translation" OFF)
MESSAGE(STATUS "UPDATE TRANSLATIONS: ${UPDATE_TRANSLATIONS}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Core)
find_package(Qt5Network)
find_package(Qt5Gui)
set(EXECUTABLE_OUTPUT_PATH bin/)

include_directories(${Qt5Core_INCLUDES} ${Qt5Gui_INCLUDES} ${Qt5Network_INCLUDES} ../)
add_definitions(${Qt5Core_DEFINITIONS} ${Qt5Gui_DEFINITIONS} ${Qt5Network_DEFINITIONS})

set(MODE "cli")

ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    # Other flags
)

IF(UPDATE_TRANSLATIONS)
    MESSAGE( update Translation )
    FILE(GLOB_RECURSE translate_dice_SRCS ../*.cpp ../*.h)
    SET(translate_SRCS ${translate_rolisteamd_SRCS})
    SET(rolisteamd_TS "${CMAKE_CURRENT_SOURCE_DIR}/i18n/rolisteamd_*.ts")
ELSE()
    MESSAGE( NO updates for translations)
    FILE(GLOB rolisteamd_TS "${CMAKE_CURRENT_SOURCE_DIR}/i18n/*.ts")
ENDIF(UPDATE_TRANSLATIONS)



if(Qt5Core_FOUND)

   # IF(UPDATE_TRANSLATIONS)
   #     MESSAGE(status  "find" ${rolisteamd_TS} ${translate_SRCS} )
  #      QT5_CREATE_TRANSLATION(rolisteamd_QM ${translate_SRCS} ${rolisteamd_TS})
 #   ELSE()
        #QT5_ADD_TRANSLATION(rolisteamd_QM ${rolisteamd_TS})
#    ENDIF()

    QT5_ADD_RESOURCES(rolisteamd_RESOURCES_RCC ${rolisteam_RESOURCES})

    # guess plugins and libraries directory
    set(QT_PLUGINS_DIR "${Qt5Core_DIR}/../../../plugins")
    get_target_property(QT_LIBRARY_DIR Qt5::Core LOCATION)
    get_filename_component(QT_LIBRARY_DIR ${QT_LIBRARY_DIR} PATH)
endif()

#networkmanager.cpp
#networklink.cpp
#    networkmessage.cpp
#    networkmessagereader.cpp
#    networkmessagewriter.cpp
#receiveevent.cpp
SET( rolisteamd_sources
    main.cpp
    tcpclient.cpp
    servermanager.cpp
    rolisteamdaemon.cpp
    connectionprofile.cpp
    channelmodel.cpp
    )



add_executable( rolisteamd ${rolisteamd_sources} ${rolisteamd_QM}   )

target_link_libraries(rolisteamd ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Network_LIBRARIES})
INSTALL_TARGETS(/bin rolisteamd )
