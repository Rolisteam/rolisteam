cmake_minimum_required(VERSION 3.16)

project(InstantMessaging VERSION 1.0.0 DESCRIPTION "Instant messaging code")


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add a compiler flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(Qt6 COMPONENTS Core Widgets Network Qml Quick Concurrent REQUIRED)


#chatroomfactory.cpp
#chatroom.cpp
#filterinstantmessagingmodel.cpp
#messagemodel.cpp
#chatroomsplittermodel.cpp
#filteredplayermodel.cpp
#instantmessagingmodel.cpp

set(im_sources
  src/messagefactory.cpp
  src/commandmessage.cpp
  src/errormessage.cpp
  src/dicemessage.cpp
  src/textwritercontroller.cpp
  src/messageinterface.cpp
  src/textmessage.cpp)

#chatroomfactory.h
#filterinstantmessagingmodel.h
#messagemodel.h
#chatroom.h
#chatroomsplittermodel.h
#filteredplayermodel.h
#instantmessagingmodel.h

set(hdir "include/instantmessaging")
set(im_headers
    ${hdir}/im_global.h
    ${hdir}/messagefactory.h
    ${hdir}/commandmessage.h
    ${hdir}/errormessage.h
    ${hdir}/dicemessage.h
    ${hdir}/textwritercontroller.h
    ${hdir}/messageinterface.h
    ${hdir}/textmessage.h)



add_library(instantmessaging SHARED ${im_headers} ${im_sources})
target_link_libraries(instantmessaging PUBLIC Qt6::Core Qt6::Network Qt6::Qml Qt6::Quick Qt6::Concurrent PRIVATE common_core diceparser_shared diceparser_qobject)
set_target_properties(instantmessaging PROPERTIES PUBLIC_HEADER "${im_headers}")
target_compile_definitions(instantmessaging PRIVATE IM_LIBRARY)

target_include_directories(instantmessaging
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)


#include(GNUInstallDirs)

#install(TARGETS instantmessaging
#  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(install_helper)
rinstallLib(instantmessaging rolisteam)
