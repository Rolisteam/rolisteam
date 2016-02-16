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

SOURCES += main.cpp\
        mainwindow.cpp \
    field.cpp \
    canvas.cpp \
    fieldmodel.cpp \
    item.cpp \
    borderlisteditor.cpp \
    qmlhighlighter.cpp \
    charactersheetbutton.cpp \
    csitem.cpp\
    rolisteamimageprovider.cpp \
    charactermodel.cpp

HEADERS  += mainwindow.h \
    field.h \
    canvas.h \
    fieldmodel.h \
    item.h \
    borderlisteditor.h \
    rolisteamimageprovider.h \
    qmlhighlighter.h \
    charactersheetbutton.h \
    csitem.h
    charactermodel.h \

FORMS    += mainwindow.ui

DISTFILES += \
    examples/Rcse/Field.qml\
    examples/cs.qml

RESOURCES += \
    qmlfile.qrc
