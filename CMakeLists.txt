cmake_minimum_required(VERSION 3.16)

project(rolisteam VERSION 1.10.0 LANGUAGES CXX)

if (MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /experimental:external /external:W0 ")
    set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "/experimental:external /external:I ")
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_CURRENT_BINARY_DIR ON)
set(CMAKE_AUTORCC ON)
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

option(CODE_COVERAGE OFF)
option(DEBUG_MODE "enable more debug report" OFF)
option(WARNING_AS_ERROR "Set all warning as error" OFF)


find_package(Qt6 COMPONENTS Core Widgets Gui LinguistTools QmlImportScanner REQUIRED)
configure_file(version.h.in version.h @ONLY)

add_subdirectory(translations)
add_subdirectory(src)

if(EXISTS ${IFW_PATH})
  include(CPackHelper)
endif()

set(documentation README.md HelpMe.md)

configure_file(scripts/generate_coverage.sh.in generate_coverage.sh @ONLY)


if(EXISTS ${IFW_PATH})
set(icons
    resources/rolisteam/logo/rolisteam.ico
    resources/rcse/logo/rcse.ico
    resources/mindmap/logo/mindmap.ico)

install(FILES ${icons} DESTINATION logo COMPONENT rolisteamComp)
endif()
