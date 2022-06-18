# CPACK
function(setupCPack ifwpath)
  set(CPACK_GENERATOR "IFW")
  set(CPACK_IFW_ROOT ${ifwpath})
  set(CPACK_PACKAGE_NAME "Rolisteam")
  set(CPACK_PACKAGE_VENDOR "Rolisteam.org")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Pen and Paper virtual tabletop Software.")
  set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
  set(CPACK_PACKAGE_HOMEPAGE_URL "https://www.rolisteam.org")
  set(CPACK_PACKAGE_CONTACT "rolisteam-org developers <www@rolisteam.org>")
  set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources/rolistheme/org.rolisteam.Rolisteam.svg")
  set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
  set(CPACK_PACKAGE_VERSION_MAJOR ${rolisteam_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${rolisteam_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${rolisteam_VERSION_PATCH})
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.txt")
  set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
  set(CPACK_IFW_PACKAGE_NAME "Installer Rolisteam")
  set(CPACK_IFW_PACKAGE_TITLE "Rolisteam")
  set(CPACK_IFW_PACKAGE_PUBLISHER "Rolisteam")
  set(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")
  set(CPACK_IFW_PACKAGE_WINDOW_ICON "${CMAKE_SOURCE_DIR}/resources/rolistheme/500-symbole.png")
  set(CPACK_IFW_PACKAGE_BANNER "${CMAKE_SOURCE_DIR}/resources/rolistheme/ifwbanner.png")

  if(WIN32)
    set(CPACK_IFW_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources/rolistheme/128.ico")
  elseif(APPLE)
    set(CPACK_IFW_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources/rolistheme/rolisteam.icns")
  endif()

  if(WIN32)
      include(InstallRequiredSystemLibraries)
      find_program(WINDEPLOYQT windeployqt HINTS "${_qt_bin_dir}")
      configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/deploy-qt-windows.cmake.in" "${CMAKE_CURRENT_SOURCE_DIR}/deploy-qt-windows.cmake" @ONLY)

      set(CPACK_PRE_BUILD_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/deploy-qt-windows.cmake)
  endif()
  include(CPack)
  include(CPackIFW)

  cpack_add_component(rolisteam DISPLAY_NAME "Rolisteam" DEPENDS utils network diceparser)
  cpack_add_component(utils)
  cpack_add_component(network)
  cpack_add_component(diceparser DISPLAY_NAME "Dice CLI diceparser")
  cpack_add_component(dice DISPLAY_NAME "Dice CLI diceparser" DEPENDS diceparser)
  cpack_add_component(roliserver DISPLAY_NAME "Rolisteam's Headless Server" DEPENDS network utils)


  cpack_ifw_configure_component("rolisteam"
      DISPLAY_NAME
          "Rolisteam"
      DESCRIPTION
          "Main application to play TTRPG with friends"
      SORTING_PRIORITY 1000
      VERSION ${PROJECT_VERSION}
      LICENSES "GPLv2" "${CMAKE_SOURCE_DIR}/COPYING.txt"
  )

endfunction()




function(rinstallLib libname comp)
  if(WIN32)
    install(TARGETS ${libname} LIBRARY DESTINATION . ARCHIVE DESTINATION . RUNTIME DESTINATION . PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${comp})
  elseif(UNIX AND NOT APPLE)
    install(TARGETS ${libname} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${comp})
  endif()
endfunction()

function(rinstallRT libname comp)
  if(WIN32)
    install(TARGETS ${libname} LIBRARY DESTINATION . ARCHIVE DESTINATION . RUNTIME DESTINATION . PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${comp})
  elseif(UNIX AND NOT APPLE)
    install(TARGETS ${libname} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${comp})
  endif()
endfunction()
