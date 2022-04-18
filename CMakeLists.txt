cmake_minimum_required(VERSION 3.16)

project(rolisteam VERSION 1.10.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DVERSION_MAJOR=1 -DVERSION_MIDDLE=10 -DVERSION_MINOR=0 -DHAVE_SOUND -DHAVE_QT_NETWORK=1 -DHAVE_WEBVIEW=1)
find_package(Qt6 COMPONENTS Core Widgets Gui LinguistTools QmlImportScanner REQUIRED)
set(CMAKE_AUTORCC ON)
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#helper
#include(cmake/TranslationHelper.cmake)
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

option(WITH_PDF "enable PDF feature" OFF)
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



if(DEBUG_MODE)
    add_definitions(-DDEBUG_MODE=1)
endif()

add_subdirectory(translations)
add_subdirectory(src)

set(documentation README.md HelpMe.md)







include(cmake/install.cmake)
