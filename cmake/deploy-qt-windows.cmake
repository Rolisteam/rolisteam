set(WINDEPLOYQT "C:/Qt/6.6.0/msvc2019_64/bin/windeployqt.exe")

set(COMPONENT_NAME_MAIN "rolisteamComp")
set(package_install "")
set(CMAKE_INSTALL_PREFIX "C:/Users/renaud/Documents/applications/build-rolisteam-Desktop_Qt_6_6_0_MSVC2019_64bit-Release/install")
set(CMAKE_CURRENT_SOURCE_DIR "C:/Users/renaud/Documents/applications/rolisteam")
set(CMAKE_BINARY_DIR "C:/Users/renaud/Documents/applications/build-rolisteam-Desktop_Qt_6_6_0_MSVC2019_64bit-Release")
set(PROJECT_BINARY_DIR "C:/Users/renaud/Documents/applications/build-rolisteam-Desktop_Qt_6_6_0_MSVC2019_64bit-Release")
set(CMAKE_CURRENT_BINARY_DIR "ON")
set(PROJECT_SOURCE_DIR "C:/Users/renaud/Documents/applications/rolisteam")


set(CMAKE_BINARY_DIR  ${dir})
file(GLOB BinaryFile LIST_DIRECTORY false RELATIVE ${CMAKE_INSTALL_PREFIX} "${CMAKE_INSTALL_PREFIX}/*.exe" "${CMAKE_INSTALL_PREFIX}/*.dll")

while(BinaryFile)
    list(GET BinaryFile 0 _binary)
    execute_process(COMMAND ${WINDEPLOYQT} --qmldir ${CMAKE_CURRENT_SOURCE_DIR} --dir ${CPACK_TEMPORARY_INSTALL_DIRECTORY}/packages/${COMPONENT_NAME_MAIN}/data/${_dest} ${CMAKE_INSTALL_PREFIX}/${_binary}) #OUTPUT_VARIABLE _output OUTPUT_STRIP_TRAILING_WHITESPACE
    if(${_binary} MATCHES ".*\\.exe$" )
        STRING(REPLACE ".exe" "" name ${_binary})
        set(StartMenuDir "@StartMenuDir@")
        set(TargetDir "@TargetDir@")
        set(DesktopDir "@DesktopDir@")
        set(Binary ${name})
        set(IconLogo ${name})
        configure_file(${PROJECT_SOURCE_DIR}/cmake/installscript.qs.in ${CPACK_TEMPORARY_INSTALL_DIRECTORY}/packages/${name}Comp/meta/installscript.qs)
    endif()
    list(REMOVE_AT BinaryFile 0)
endwhile()



