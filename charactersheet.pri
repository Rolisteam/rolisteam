HEADERS += $$PWD/charactersheetwindow.h \
    $$PWD/charactersheetmodel.h\
    $$PWD/charactersheet.h \
    $$PWD/charactersheetitem.h \
    $$PWD/charactersheetbutton.h \
    $$PWD/field.h\
    $$PWD/csitem.h\
    $$PWD/section.h \
    $$PWD/rolisteamimageprovider.h \
    $$PWD/formula/formulamanager.h \
    $$PWD/formula/parsingtoolformula.h \
    $$PWD/formula/nodes/formulanode.h \
    $$PWD/formula/nodes/startnode.h \
    $$PWD/formula/nodes/operator.h


SOURCES += $$PWD/charactersheetwindow.cpp \
    $$PWD/charactersheetmodel.cpp\
    $$PWD/charactersheet.cpp \
    $$PWD/charactersheetbutton.cpp \
    $$PWD/charactersheetitem.cpp \
    $$PWD/section.cpp \
    $$PWD/field.cpp\
    $$PWD/csitem.cpp\
    $$PWD/rolisteamimageprovider.cpp \
    $$PWD/formula/formulamanager.cpp \
    $$PWD/formula/parsingtoolformula.cpp \
    $$PWD/formula/nodes/formulanode.cpp \
    $$PWD/formula/nodes/startnode.cpp \
    $$PWD/formula/nodes/operator.cpp


INCLUDEPATH += $$PWD formula formula/nodes

RCSE{
HEADERS -=$$PWD/charactersheetwindow.h
SOURCES -=$$PWD/charactersheetwindow.cpp
}
