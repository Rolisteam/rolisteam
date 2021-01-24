cmake_minimum_required(VERSION 3.10)

project(rolisteam VERSION 1.10.0 LANGUAGES CXX)
add_definitions(-DVERSION_MAJOR=1 -DVERSION_MIDDLE=10 -DVERSION_MINOR=0 -DHAVE_SOUND -DHAVE_QT_NETWORK=1 -DHAVE_WEBVIEW=1)
set(QT_REQUIRED_VERSION "5.15.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets Gui LinguistTools)

#helper
#include(cmake/TranslationHelper.cmake)
option(TESTS "enable tests compilation" OFF)
option(WITH_PDF "enable PDF feature" ON)
option(DEBUG_MODE "enable more debug report" OFF)
option(WARNING_AS_ERROR "Set all warning as error" OFF)


add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

add_library(project_warnings INTERFACE)

IF (WARNING_AS_ERROR)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)
endif()

include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

include(cmake/StaticAnalyzers.cmake)

if (TESTS)
enable_testing(true)
add_subdirectory(tests)
endif()

if(DEBUG_MODE)
    add_definitions(-DDEBUG_MODE=1)
endif()

add_subdirectory(translations)
add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(server)


set(documentation README.md HelpMe.md)



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
