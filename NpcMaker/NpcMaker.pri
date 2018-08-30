FORMS += \
    $$PWD/npcmakerwidget.ui

HEADERS += \
    $$PWD/npcmakerwidget.h \
    $$PWD/genericmodel.h

SOURCES += \
    $$PWD/npcmakerwidget.cpp \
    $$PWD/genericmodel.cpp


UNIT_TEST {
SOURCES -= $$PWD/npcmakerwidget.cpp
HEADERS -= $$PWD/npcmakerwidget.h
}
