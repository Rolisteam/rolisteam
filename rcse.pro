#-------------------------------------------------
#
# Project created by QtCreator 2016-01-04T16:16:31
#
#-------------------------------------------------

QT       += core gui quickwidgets quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rcse
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    field.cpp \
    canvas.cpp \
    fieldmodel.cpp \
    item.cpp \
    borderlisteditor.cpp

HEADERS  += mainwindow.h \
    field.h \
    canvas.h \
    fieldmodel.h \
    item.h \
    borderlisteditor.h

FORMS    += mainwindow.ui

DISTFILES += \
    field.qml
