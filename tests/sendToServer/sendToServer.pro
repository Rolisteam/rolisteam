SOURCES += \
    main.cpp \
    readersender.cpp

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QT += core network
TARGET = tst_sendToServer

HEADERS += \
    readersender.h
