TEMPLATE = app
QT += core network
QT -= gui


HEADERS += \
    servermanager.h \
    server.h


SOURCES += \
    servermanager.cpp \
    server.cpp \
    main.cpp

UI_DIR = ../ui
MOC_DIR = ../obj
OBJECTS_DIR = ../obj
TARGET=../bin/rolisteam-server
