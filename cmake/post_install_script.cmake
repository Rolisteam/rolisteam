
set(COMPONENT_NAME_MAIN "@CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT@")
set(package_install "")
set(PROJECT_BINARY_DIR "@PROJECT_BINARY_DIR@")
set(PROJECT_SOURCE_DIR "@PROJECT_SOURCE_DIR@")


set(CMAKE_BINARY_DIR  ${dir})
file(GLOB executableFile LIST_DIRECTORY false RELATIVE ${CMAKE_INSTALL_PREFIX} "${CMAKE_INSTALL_PREFIX}/*.exe")
message("++++++++++++++ ${executableFile} ***")

while(executableFile)
    list(GET executableFile 0 _exec)

    STRING(REPLACE ".exe" "" name ${_exec})
    file(READ ${CPACK_TEMPORARY_INSTALL_DIRECTORY}/packages/${name}Comp/meta/package.xml FILE_CONTENTS)
    string(REPLACE "</Package>" "    <Script>installscript.qs</Script>\r\n</Package>" RIGHT_CONTENTS ${FILE_CONTENTS})
    file(WRITE ${CPACK_TEMPORARY_INSTALL_DIRECTORY}/packages/${name}Comp/meta/package.xml ${RIGHT_CONTENTS})

    list(REMOVE_AT executableFile 0)
endwhile()

