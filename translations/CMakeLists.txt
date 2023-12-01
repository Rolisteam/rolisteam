if(CMAKE_BUILD_TYPE STREQUAL "Release" OR ${GENERATE_TS_FILES})
    file(GLOB QM_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.qm")
    qt6_add_resources(rolisteam translations
        PREFIX "/translations"
        FILES ${QM_FILES}
        )
    qt6_add_resources(rcse translations
        PREFIX "/translations"
        FILES ${QM_FILES}
        )
    qt6_add_resources(mindmapbin translations
        PREFIX "/translations"
        FILES ${QM_FILES}
        )
    qt6_add_resources(roliserver translations
        PREFIX "/translations"
        FILES ${QM_FILES}
        )
    qt6_add_resources(dice translations
        PREFIX "/translations"
        FILES ${QM_FILES}
        )
    qt6_add_resources(tst_models translations
        PREFIX "/translations"
        FILES ${QM_FILES}
        )
    add_dependencies(rolisteam BuildTranslations)
endif()
