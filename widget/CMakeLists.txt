cmake_minimum_required(VERSION 3.10)

set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOMOC_MOC_OPTIONS "-nw")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(QT_REQUIRED_VERSION "5.15.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets Gui Svg)

#if(TESTS)
#    find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test)
#endif()


set(HEADERS
  campaignproperties.h

  customs/colorlisteditor.h
  customs/imagepatheditor.h

  delegates/checkboxdelegate.h
  delegates/colordelegate.h
  delegates/imagepathdelegateitem.h
)


set(SOURCES
    campaignproperties.cpp
    customs/colorlisteditor.cpp
    customs/imagepatheditor.cpp

    delegates/checkboxdelegate.cpp
    delegates/colordelegate.cpp
    delegates/imagepathdelegateitem.cpp
)


set(UI_FILES
  campaignproperties.ui
)

set(QRC_SOURCES ../rolisteam.qrc)

add_library(rwidgets SHARED ${SOURCES} ${HEADERS} ${UI_FILES} ${QRC_SOURCES})
target_link_libraries(rwidgets PUBLIC project_warnings project_options Qt5::Core Qt5::Gui Qt5::Svg Qt5::Widgets PRIVATE core)


target_include_directories(rwidgets PUBLIC src)
target_include_directories(rwidgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

message(${CMAKE_CURRENT_BINARY_DIR})

#set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/core_autogen/mocs_compilation.cpp" PROPERTIES COMPILE_FLAGS "-Wno-undefined-reinterpret-cast -Wno-useless-cast")
#set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/network_autogen/mocs_compilation.cpp" PROPERTIES COMPILE_FLAGS "-Wno-undefined-reinterpret-cast -Wno-useless-cast")

install(TARGETS rwidgets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)
