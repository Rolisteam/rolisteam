#Functions
#function(convertTs2Qm _inFile _result)
#    string(REPLACE ".ts" ".qm" _qmFile ${_inFile})
#    set(_result ${_qmFile} PARENT_SCOPE)
#endfunction()

# Options
option(TRANSLATIONS "generate QM translation" ON)
option(UPDATE_TRANSLATIONS "generate TS translation files" OFF)

# PATHS
#set(tsRoot ${CMAKE_CURRENT_SOURCE_DIR}/translations)
#set(poRoot ${CMAKE_CURRENT_SOURCE_DIR}/po)

set(TS_FILES rolisteam_hu_HU.ts
    rolisteam_it.ts
    rolisteam_nl_NL.ts
    rolisteam_ca.ts
    rolisteam_pt_BR.ts
    rolisteam_de.ts
    rolisteam_ro_RO.ts
    rolisteam_es.ts
    rolisteam_tr.ts
    rolisteam_fr.ts
    rolisteam.ts)


IF(UPDATE_TRANSLATIONS)
    qt5_create_translation(TS_PATH_FILES ${CMAKE_SOURCE_DIR}/client ${CMAKE_SOURCE_DIR}/core ${CMAKE_SOURCE_DIR}/server  ${TS_FILES} OPTION -source-language en_US)
ENDIF()

IF(TRANSLATIONS)
    set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${CMAKE_CURRENT_SOURCE_DIR})
    qt5_add_translation(QM_FILES ${TS_FILES})
    add_custom_target(translations ALL DEPENDS ${QM_FILES})
ENDIF()



#foreach(TsFile ${translationFiles})
#    string(REPLACE ".ts" ".qm" _qmOutput ${TsFile})
#    string(REPLACE ".ts" ".po" _poOutput ${TsFile})
#    string(REPLACE ${tsRoot} ${poRoot} _poOutput ${_poOutput})
#    add_custom_command(OUTPUT ${_qmOutput}
#        COMMAND cmake -E make_directory ${poRoot}
#        #Get po from svn
#        COMMAND Qt5::lconvert -if po -of ts -i ${_poOutput} -o ${TsFile}
#        COMMAND Qt5::lupdate ${CMAKE_CURRENT_SOURCE_DIR} -ts ${TsFile}
#        COMMAND Qt5::lrelease -compress -nounfinished ${TsFile} -qm ${_qmOutput}
#        COMMAND Qt5::lconvert -if ts -of po -i ${TsFile} -o ${_poOutput})
#        #Push updated po
#    list(APPEND QM_FILES ${_qmOutput})
#endforeach()
#add_custom_target(BuildTranslations
#    ALL DEPENDS ${QM_FILES})
