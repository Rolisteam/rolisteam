QT       += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = NameGenerator

FORMS += \
    namegeneratorwidget.ui

HEADERS += \
    namegeneratorwidget.h

SOURCES += \
    namegeneratorwidget.cpp \
    main.cpp

RESOURCES += \
    resources/resources.qrc


