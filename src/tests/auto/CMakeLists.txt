cmake_minimum_required(VERSION 3.16)

include_directories(../libraries)
include_directories(../libraries/diceparser/include)

# Helpers
add_subdirectory(helper)

# CMakeLists.txt
#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)

#subdirs with tests
add_subdirectory(audioplayer)
add_subdirectory(antaFilter)
add_subdirectory(preferences )
add_subdirectory(colorButton)
add_subdirectory(contentmodel)
add_subdirectory(instantmessaging)
add_subdirectory(paletteModel)
add_subdirectory(chat)
add_subdirectory(rolisteamtheme )
add_subdirectory(picture )
add_subdirectory(session )
add_subdirectory(data)
add_subdirectory(fieldModelTest )
add_subdirectory(updater )
add_subdirectory(controller )
add_subdirectory(playermodel)
add_subdirectory(campaign)
add_subdirectory(mindmap)
add_subdirectory(vmap )
add_subdirectory(serialization )
add_subdirectory(widgets )
add_subdirectory(network )
add_subdirectory(musicmodel)
add_subdirectory(channelmodel)
add_subdirectory(characterstatemodel)
add_subdirectory(dicealiasmodel)
add_subdirectory(genericmodel)
add_subdirectory(charactermodel)
add_subdirectory(unitmodel)
add_subdirectory(worker)
add_subdirectory(qobjects)
add_subdirectory(services)
add_subdirectory(noteeditor)
add_subdirectory(models)
add_subdirectory(delegate)


if(CMAKE_COMPILER_IS_GNUCXX AND CODE_COVERAGE)
    set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
    include(CodeCoverage)
    append_coverage_compiler_flags_to_target(core)
    append_coverage_compiler_flags_to_target(rwidgets)
    append_coverage_compiler_flags_to_target(common_core)
    append_coverage_compiler_flags_to_target(common_widget)
    append_coverage_compiler_flags_to_target(common_qml)
    append_coverage_compiler_flags_to_target(instantmessaging)
    append_coverage_compiler_flags_to_target(charactersheet_widgets)
    append_coverage_compiler_flags_to_target(utils)
    append_coverage_compiler_flags_to_target(rmindmap)
    append_coverage_compiler_flags_to_target(qml_views)
    append_coverage_compiler_flags_to_target(qml_components)
    append_coverage_compiler_flags_to_target(network)

    setup_target_for_coverage_gcovr_html(
      NAME tests_coverage
      EXECUTABLE tests_coverage
      DEPENDENCIES core rwidgets common_core common_widget common_qml instantmessaging charactersheet_widgets diceparser_qobject utils rmindmap qml_views qml_components network
      BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src"
      EXCLUDE "bin/*")
endif()
