include(../tests.pri)

TEMPLATE = app
TARGET=MainWindowTest
MOC_DIR=trash
OBJECTS_DIR=trash
CONFIG+=testcase


SOURCES += tst_mainwindow.cpp



QT += testlib widgets network quick qml

SOURCES += $$ROLISTEAMSRC/mainwindow.cpp

HEADERS += $$ROLISTEAMSRC/mainwindow.h
