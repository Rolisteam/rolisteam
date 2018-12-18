include(../tests.pri)

TARGET = tst_widgets
CONFIG   += console testcase
CONFIG   -= app_bundle

QT += widgets gui

TEMPLATE = app

HEADERS +=  $$ROLISTEAMSRC/widgets/circledisplayer.h \
            $$ROLISTEAMSRC/widgets/colorselector.h\
            $$ROLISTEAMSRC/widgets/diameterselector.h \
            $$ROLISTEAMSRC/widgets/filedirchooser.h\
            $$ROLISTEAMSRC/widgets/realslider.h \
            $$ROLISTEAMSRC/widgets/flowlayout.h \
            $$ROLISTEAMSRC/preferences/preferencesmanager.h \
            $$ROLISTEAMSRC/data/cleveruri.h \
            $$ROLISTEAMSRC/data/resourcesnode.h \


SOURCES += \
    tst_widgets.cpp \
    $$ROLISTEAMSRC/widgets/circledisplayer.cpp \
    $$ROLISTEAMSRC/widgets/colorselector.cpp\
    $$ROLISTEAMSRC/widgets/diameterselector.cpp \
    $$ROLISTEAMSRC/widgets/filedirchooser.cpp\
    $$ROLISTEAMSRC/widgets/realslider.cpp \
    $$ROLISTEAMSRC/widgets/flowlayout.cpp \
    $$ROLISTEAMSRC/preferences/preferencesmanager.cpp \
    $$ROLISTEAMSRC/data/cleveruri.cpp \
    $$ROLISTEAMSRC/data/resourcesnode.cpp \


INCLUDEPATH += $$ROLISTEAMSRC/widgets/
