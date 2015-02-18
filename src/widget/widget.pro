CONFIG      += designer plugin
 TARGET      = pathchooserplugin
 #$$qtLibraryTarget($$TARGET)
 TEMPLATE    = lib
 QTDIR_build:DESTDIR     = $$QT_BUILD_TREE/plugins/designer

 HEADERS     =     pathchooser.h \
                pathchooserplugin.h
 SOURCES     =     pathchooser.cpp \
                pathchooserplugin.cpp

 # install
 target.path = $$[QT_INSTALL_PLUGINS]/designer
 sources.files = $$SOURCES $$HEADERS *.pro
 sources.path = $$[QT_INSTALL_EXAMPLES]/designer/pathchooserplugin
 INSTALLS += target sources
