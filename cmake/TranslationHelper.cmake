file(GLOB Langs RELATIVE "${PROJECT_SOURCE_DIR}/poqm/" "${PROJECT_SOURCE_DIR}/poqm/*")
set(PO_FILES "")
set(TS_FILES "")
set(QM_FILES "")
foreach(lang ${Langs})
    set(i18n_ROOT_DIR "${PROJECT_SOURCE_DIR}/translations/")
    set(PO_FILE ${PROJECT_SOURCE_DIR}/poqm/${lang}/rolisteam_qt.po) #${PO_FILES}
    set(TS_FILE ${PROJECT_SOURCE_DIR}/translations/rolisteam_${lang}.ts) #${TS_FILES}
    set(QM_FILE ${PROJECT_SOURCE_DIR}/translations/rolisteam_${lang}.qm) #${QM_FILES}

    add_custom_command(OUTPUT ${QM_FILE}
            COMMAND cmake -E make_directory ${i18n_ROOT_DIR}
            #Change PO to TS, ts to QM
            COMMAND Qt6::lconvert -if po -of ts -i ${PO_FILE} -o ${TS_FILE}
            COMMAND Qt6::lrelease -compress -nounfinished ${TS_FILE} -qm ${QM_FILE})
            #Push updated po
            list(APPEND QM_FILES ${QM_FILE})
endforeach()

add_custom_target(BuildTranslations
    ALL DEPENDS ${QM_FILES})

