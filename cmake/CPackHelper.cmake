set(CPACK_GENERATOR "IFW")
STRING(REPLACE "\\" "/" CPACK_IFW_ROOT ${IFW_PATH}) # /home/renaud/application/other/Qt/Tools/QtInstallerFramework/4.2
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "Rolisteam")
set(CPACK_PACKAGE_NAME "Rolisteam")
set(CPACK_PACKAGE_VENDOR "Rolisteam.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Pen and Paper virtual tabletop Software.")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
#set(CPACK_INSTALL_CMAKE_PROJECTS )
set(CPACK_PACKAGE_HOMEPAGE_URL "https://www.rolisteam.org")
set(CPACK_PACKAGE_CONTACT "rolisteam-org developers <www@rolisteam.org>")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources/rolistheme/org.rolisteam.Rolisteam.svg")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_VERSION_MAJOR ${rolisteam_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${rolisteam_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${rolisteam_VERSION_PATCH})
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
#set(CPACK_IFW_PACKAGE_NAME "Rolisteam")
set(CPACK_IFW_PACKAGE_TITLE "Installer Rolisteam")
set(CPACK_IFW_PACKAGE_PUBLISHER "Rolisteam")
set(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")
set(CPACK_IFW_PACKAGE_WINDOW_ICON "${CMAKE_SOURCE_DIR}/resources/rolistheme/500-symbole.png")
set(CPACK_IFW_PACKAGE_BANNER "${CMAKE_SOURCE_DIR}/resources/rolistheme/ifwbanner.png")
set(CPACK_IFW_VERBOSE ON)
set(CPACK_PACKAGE_EXECUTABLES "")


if(WIN32)
  set(CPACK_IFW_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources/rolistheme/128.ico")
elseif(APPLE)
  set(CPACK_IFW_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources/rolistheme/rolisteam.icns")
endif()

if(WIN32)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT rolisteamComp)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
    include(InstallRequiredSystemLibraries)
    find_program(WINDEPLOYQT windeployqt HINTS "${_qt_bin_dir}")
    set(StartMenuDir "@StartMenuDir@")
    set(TargetDir "@TargetDir@")
    set(DesktopDir "@DesktopDir@")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/deploy-qt-windows.cmake.in" "${CMAKE_CURRENT_SOURCE_DIR}/deploy-qt-windows.cmake" @ONLY)
    set(CPACK_PRE_BUILD_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/deploy-qt-windows.cmake)    
    #set(CPACK_POST_BUILD_SCRIPTS "${CMAKE_SOURCE_DIR}/cmake/post_install_script.cmake")
endif()

include(CPack)
include(CPackIFW)

cpack_add_component(diceComp DISPLAY_NAME "Dice CLI" DEPENDS libraries)
cpack_add_component(rcseComp DISPLAY_NAME "CharacterSheet Editor (rcse)" DEPENDS libraries)
cpack_add_component(libraries DISPLAY_NAME "Libraries" REQUIRED DESCRIPTION "Dynamic libs")
cpack_add_component(rolisteamComp DISPLAY_NAME "Rolisteam" DEPENDS libraries)
cpack_add_component(roliserverComp DISPLAY_NAME "Stand alone Server" DEPENDS libraries)
cpack_add_component(mindmapComp DISPLAY_NAME "MindMap Editor" DEPENDS libraries)

cpack_ifw_configure_component(libraries
    DESCRIPTION
        "Dynamic libs"
    SORTING_PRIORITY 10
    VERSION ${PROJECT_VERSION}
    LICENSES "GPLv2" "${CMAKE_SOURCE_DIR}/COPYING.txt"
    DEFAULT "true"
)
#C:\Users\renaud\Documents\applications\build-rolisteam-Desktop_Qt_6_6_0_MSVC2019_64bit-Release\_CPack_Packages\win64\IFW\Rolisteam-1.10.0-win64\packages\rolisteamComp\meta\installscript.qs
cpack_ifw_configure_component(rolisteamComp
    DESCRIPTION
        "Main application to play TTRPG with friends"
    VERSION ${PROJECT_VERSION}
    SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/rolisteam_installscript.qs"
    SORTING_PRIORITY 1000
    LICENSES "GPLv2" "${CMAKE_SOURCE_DIR}/COPYING.txt"
    DEFAULT "true"
)

cpack_ifw_configure_component(rcseComp
    DESCRIPTION
        "Rolisteam CharacterSheet Editor"
    SORTING_PRIORITY 999
    VERSION ${PROJECT_VERSION}
    SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/rcse_installscript.qs"
    LICENSES "GPLv2" "${CMAKE_SOURCE_DIR}/COPYING.txt"
    DEFAULT "true"
)

cpack_ifw_configure_component(roliserverComp
    DESCRIPTION
        "Standalone and headless rolisteam server"
    SORTING_PRIORITY 997
    #SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/roliserver_installscript.qs"
    VERSION ${PROJECT_VERSION}
    LICENSES "GPLv2" "${CMAKE_SOURCE_DIR}/COPYING.txt"
    DEFAULT "true"
)

cpack_ifw_configure_component(mindmapComp
  DESCRIPTION
      "Standalone mindmap application"
  SORTING_PRIORITY 998
  SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/mindmap_installscript.qs"
  VERSION ${PROJECT_VERSION}
  LICENSES "GPLv3" "${CMAKE_SOURCE_DIR}/src/libraries/diceparser/LICENSE"
  DEFAULT "true"
)

cpack_ifw_configure_component(diceComp
    DESCRIPTION
        "Command line tool to roll Dice"
    SORTING_PRIORITY 990    
    #SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cmake/dice_installscript.qs"
    VERSION ${PROJECT_VERSION}
    LICENSES "GPLv3" "${CMAKE_SOURCE_DIR}/src/libraries/diceparser/LICENSE"
    DEFAULT "true"
)
