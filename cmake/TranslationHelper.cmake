#Functions
function(convertTs2Po _inFile _outFile)


endfunction()



function(convertPo2Ts _inFile _outFile)
    #string(REPLACE ".po" ".ts" _outFile ${_inFile})

endfunction()



function(convertTs2Qm _inFile _result)
    string(REPLACE ".ts" ".qm" _qmFile ${_inFile})
    set(_result ${_qmFile} PARENT_SCOPE)
endfunction()

# Options
option(TRANSLATIONS "generate QM translation" ON)
option(UPDATE_TRANSLATIONS "generate TS translation files" OFF)

# PATHS
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


IF(UPDATE_TRANSLATIONS)
qt5_create_translation(TS_FILES ${CMAKE_SOURCE_DIR}/client ${CMAKE_SOURCE_DIR}/core ${CMAKE_SOURCE_DIR}/server  ${translationFiles} OPTION -source-language en_US)
ENDIF()

IF(TRANSLATIONS)
    qt5_add_translation(qmFiles ${translationFiles})
    add_custom_target(translations DEPENDS ${qmFiles})
ENDIF()



qt5_add_resources(translations_RCC ${qmFiles})



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
