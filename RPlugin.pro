
include(NameGenerator/NameGenerator.pri)
include(UnitConvertor/UnitConvertor.pri)

SOURCES += \
    main.cpp


TEMPLATE = app

TARGET = rplugin

QT += gui widgets

CONFIG += c++11
