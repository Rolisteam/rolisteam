add_definitions(-DHAVE_SOUND -DHAVE_QT_NETWORK=1 -DHAVE_WEBVIEW=1)
if(DEBUG_MODE)
    add_definitions(-DDEBUG_MODE=1)
endif()


if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

IF (WARNING_AS_ERROR)
  include(CompilerWarnings)
  set_project_warnings(rolisteam)
  include(Sanitizers)
  enable_sanitizers(rolisteam)
  include(StaticAnalyzers)
endif()



add_subdirectory(libraries)
add_subdirectory(binaries)


if (BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
