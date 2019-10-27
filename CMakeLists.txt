cmake_minimum_required(VERSION 3.10)

project(rolisteam)
add_definitions(-DVERSION_MAJOR=1 -DVERSION_MIDDLE=9 -DVERSION_MINOR=0)
set(QT_REQUIRED_VERSION "5.12.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets Gui LinguistTools)

#helper
include(cmake/TranslationHelper.cmake)


add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(server)

if (TESTS)
add_subdirectory(tests)
endif()



set(documentation README.md HelpMe.md)


set(tsRoot ${CMAKE_CURRENT_SOURCE_DIR}/translations)
set(poRoot ${CMAKE_CURRENT_SOURCE_DIR}/po)

set(translationFiles ${tsRoot}/rolisteam_hu_HU.ts
    ${tsRoot}/rolisteam_it.ts
    ${tsRoot}/rolisteam_nl_NL.ts
    ${tsRoot}/rolisteam_ca.ts
    ${tsRoot}/rolisteam_pt_BR.ts
    ${tsRoot}/rolisteam_de.ts
    ${tsRoot}/rolisteam_ro_RO.ts
    ${tsRoot}/rolisteam_es.ts
    ${tsRoot}/rolisteam_tr.ts
    ${tsRoot}/rolisteam_fr.ts
    ${tsRoot}/rolisteam.ts)

foreach(TsFile ${translationFiles})
    string(REPLACE ".ts" ".qm" _qmOutput ${TsFile})
    string(REPLACE ".ts" ".po" _poOutput ${TsFile})
    string(REPLACE ${tsRoot} ${poRoot} _poOutput ${_poOutput})
    add_custom_command(OUTPUT ${_qmOutput} 
        COMMAND cmake -E make_directory ${poRoot}
        #Get po from svn
        COMMAND Qt5::lconvert -if po -of ts -i ${_poOutput} -o ${TsFile}
        COMMAND Qt5::lupdate ${CMAKE_CURRENT_SOURCE_DIR} -ts ${TsFile}
        COMMAND Qt5::lrelease -compress -nounfinished ${TsFile} -qm ${_qmOutput}
        COMMAND Qt5::lconvert -if ts -of po -i ${TsFile} -o ${_poOutput})
        #Push updated po
    list(APPEND QM_FILES ${_qmOutput})
endforeach()
add_custom_target(BuildTranslations
    ALL DEPENDS ${QM_FILES})
