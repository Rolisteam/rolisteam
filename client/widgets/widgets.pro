CONFIG      += designer plugin
TARGET      = $$qtLibraryTarget(widgets)
TEMPLATE    = lib

QTDIR_build:DESTDIR     = $$QT_BUILD_TREE/plugins/designer

HEADERS     = dirchooser.h \
           dirchooserplugin.h
SOURCES     = dirchooser.cpp \
           dirchooserplugin.cpp

build_all:!build_pass {
 CONFIG -= build_all
 CONFIG += release
}

# install
target.path = $$[QT_INSTALL_PLUGINS]/designer

INSTALLS += target

symbian: include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
