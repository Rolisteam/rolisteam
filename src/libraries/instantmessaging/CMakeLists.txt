cmake_minimum_required(VERSION 3.14)

project(InstantMessaging VERSION 1.0.0 DESCRIPTION "Instant messaging code")


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Add a compiler flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(QT_REQUIRED_VERSION "5.15.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets Network Qml Quick Concurrent)




set(im_sources
avatarprovider.cpp
chatroomfactory.cpp
commandmessage.cpp
errormessage.cpp
filterinstantmessagingmodel.cpp
messagefactory.cpp
messagemodel.cpp
textwritercontroller.cpp
chatroom.cpp
chatroomsplittermodel.cpp
dicemessage.cpp
filteredplayermodel.cpp
instantmessagingmodel.cpp
messageinterface.cpp
textmessage.cpp)


set(im_headers
avatarprovider.h
chatroomfactory.h
commandmessage.h
errormessage.h
filterinstantmessagingmodel.h
messagefactory.h
messagemodel.h
textwritercontroller.h
chatroom.h
chatroomsplittermodel.h
dicemessage.h
filteredplayermodel.h
instantmessagingmodel.h
messageinterface.h
textmessage.h)



add_library(instantmessaging SHARED ${im_headers} ${im_sources})
target_link_libraries(instantmessaging PUBLIC Qt5::Core Qt5::Network Qt5::Qml Qt5::Quick Qt5::Concurrent)
target_include_directories(instantmessaging PRIVATE ../  ../diceparser ../diceparser/include)
