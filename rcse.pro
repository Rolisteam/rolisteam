#-------------------------------------------------
#
# Project created by QtCreator 2016-01-04T16:16:31
#
#-------------------------------------------------

QT       += core gui quickwidgets quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rcse
TEMPLATE = app

QMAKE_CXXFLAGS = -std=c++11

DEFINES+=RCSE
CONFIG+=RCSE

include(charactersheet/charactersheet.pri)

INCLUDEPATH += charactersheet

SOURCES += main.cpp\
        mainwindow.cpp \
    canvas.cpp \
    fieldmodel.cpp \
    borderlisteditor.cpp \
    qmlhighlighter.cpp \
    charactermodel.cpp

HEADERS  += mainwindow.h \
    canvas.h \
    fieldmodel.h \
    borderlisteditor.h \
    qmlhighlighter.h \
    charactermodel.h \

FORMS    += mainwindow.ui

DISTFILES += \
    examples/Rcse/Field.qml\
    examples/cs.qml

RESOURCES += \
    qmlfile.qrc
