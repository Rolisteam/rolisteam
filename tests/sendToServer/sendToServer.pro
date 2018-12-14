SOURCES += \
    main.cpp \
    readersender.cpp

CONFIG   += console testcase
CONFIG   -= app_bundle

TEMPLATE = app

QT += core network
TARGET = tst_sendToServer

HEADERS += \
    readersender.h
