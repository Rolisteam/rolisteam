cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)


# test player
add_executable(tst_player tst_dataplayertest.cpp)
target_link_libraries(tst_player PUBLIC Qt6::Core Qt6::Test PRIVATE core core rwidgets instantmessaging common_core common_widget common_qml network helper)
target_include_directories(tst_player PUBLIC ../helper)
add_test(NAME tst_player COMMAND tst_player)

# test chapter
#add_executable(tst_chapter tst_chaptertest.cpp)
#target_link_libraries(tst_chapter PUBLIC Qt6::Core Qt6::Test PRIVATE core helper)
#target_include_directories(tst_chapter PUBLIC ../helper)
#add_test(NAME tst_chapter COMMAND tst_chapter)

# test character
add_executable(tst_character tst_datacharactertest.cpp ../assets/resources.qrc)
target_link_libraries(tst_character
    PUBLIC
        Qt6::Core
        Qt6::Test
    PRIVATE
        core
        rwidgets
        instantmessaging
        common_core
        common_widget
        common_qml
        network
        helper
        charactersheet
        charactersheet_formula)

target_include_directories(tst_character PUBLIC ../helper)
add_test(NAME tst_character COMMAND tst_character)

# test cleveruri
add_executable(tst_cleveruri tst_datacleveruritest.cpp ../assets/resources.qrc)
target_link_libraries(tst_cleveruri PUBLIC Qt6::Core Qt6::Test PRIVATE core core rwidgets instantmessaging common_core common_widget common_qml network helper)
target_include_directories(tst_cleveruri PUBLIC ../helper)
add_test(NAME tst_cleveruri COMMAND tst_cleveruri)


# test diceshortcut
add_executable(tst_diceshortcut tst_diceshortcut.cpp)
target_link_libraries(tst_diceshortcut PUBLIC Qt6::Core Qt6::Test PRIVATE core helper)
target_include_directories(tst_diceshortcut PUBLIC ../helper)
add_test(NAME tst_diceshortcut COMMAND tst_diceshortcut)

# test resource node
add_executable(tst_resourcenode tst_resourcenode.cpp)
target_link_libraries(tst_resourcenode PUBLIC Qt6::Core Qt6::Test PRIVATE core helper)
target_include_directories(tst_resourcenode PUBLIC ../helper)
add_test(NAME tst_resourcenode COMMAND tst_resourcenode)


# test resource node
add_executable(tst_rolisteammimedata tst_rolisteammimedata.cpp)
target_link_libraries(tst_rolisteammimedata PUBLIC Qt6::Core Qt6::Test PRIVATE core helper)
target_include_directories(tst_rolisteammimedata PUBLIC ../helper)
add_test(NAME tst_rolisteammimedata COMMAND tst_rolisteammimedata)

