#-------------------------------------------------
#
# Project created by QtCreator 2016-01-04T16:16:31
#
#-------------------------------------------------




CONFIG-=console

QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]


# Version
DEFINES += VERSION_MAJOR=1 VERSION_MIDDLE=9 VERSION_MINOR=0

TEMPLATE = subdirs
SUBDIRS = src


TESTS {
    message("test")
    SUBDIRS += tests
}


MOC_DIR = bin
OBJECT_DIR = bin

UI_BIN = bin

RESOURCES += qmlfile.qrc


OTHER_FILES += AUTHORS README.md .travis.yml





