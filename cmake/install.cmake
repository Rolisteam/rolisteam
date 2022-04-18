# install
include(GNUInstallDirs)

#set(ALL_TARGETS_NAME rolisteam core networkr mindmap-gui common_core common_qml common_widget qml_views rwidget rmindmap instantmessaging roliserver dice charactersheet qml_components charactersheet_formula charactersheet_widgets)
#
#
#set(TARGETS_NAME_LIST)
#foreach(TargetName ${ALL_TARGETS_NAME})
#  if(TARGET ${TargetName})
#    set(TARGETS_NAME_LIST ${TargetName} ${TARGETS_NAME_LIST})
#  endif()
#endforeach()
#
#message("${TARGETS_NAME_LIST}")
#
#
#install(TARGETS ${TARGETS_NAME_LIST}
#    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})




# CPACK
include(CPack)
set(CPACK_PACKAGE_VENDOR "rolisteam-org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Pen and Paper virtual tabletop Software.")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_HOMEPAGE_URL "https://rolisteam.org")
set(CPACK_PACKAGE_CONTACT "rolisteam-org developers <www@rolisteam.org>")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources/rolistheme/org.rolisteam.Rolisteam.svg") # TODO: Can any generator make use of this?
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")


if(WIN32)
    include(InstallRequiredSystemLibraries)

    set(CPACK_GENERATOR WIX ZIP)
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME_CAPITALIZED}")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME_CAPITALIZED}")
    set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} "${PROJECT_NAME_CAPITALIZED}")
    set(CPACK_CREATE_DESKTOP_LINKS ${PROJECT_NAME})

                # WIX (Windows .msi installer)
    # 48x48 pixels
    set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/resources/rolistheme/128.ico")
    # Supported languages can be found at
    # http://wixtoolset.org/documentation/manual/v3/wixui/wixui_localization.html
    #set(CPACK_WIX_CULTURES "ar-SA,bg-BG,ca-ES,hr-HR,cs-CZ,da-DK,nl-NL,en-US,et-EE,fi-FI,fr-FR,de-DE")
    set(CPACK_WIX_UI_BANNER "${CMAKE_SOURCE_DIR}/resources/packaging/CPACK_WIX_UI_BANNER.BMP")
    set(CPACK_WIX_UI_DIALOG "${CMAKE_SOURCE_DIR}/resources/packaging/CPACK_WIX_UI_DIALOG.BMP")
    set(CPACK_WIX_PROPERTY_ARPHELPLINK "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "${CPACK_PACKAGE_HOMEPAGE_URL}")
    set(CPACK_WIX_ROOT_FEATURE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
    set(CPACK_WIX_LIGHT_EXTRA_FLAGS "-dcl:high") # set high compression


    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.txt")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

    # The correct way would be to include both x32 and x64 into one installer
    # and install the appropriate one.
    # CMake does not support that, so there are two separate GUID's
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(CPACK_WIX_UPGRADE_GUID "26D8062A-66D9-48D9-8924-42090FB9B3F9")
    else()
      set(CPACK_WIX_UPGRADE_GUID "2C53E1B9-51D9-4429-AAE4-B02221959AA5")
    endif()
elseif(APPLE)
  set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-osx")
  set(CPACK_GENERATOR ZIP)
else()
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-linux")
  set(CPACK_GENERATOR TGZ)
  set(CPACK_SOURCE_GENERATOR TGZ)
endif()
