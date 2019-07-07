QT += quickwidgets

HEADERS += $$PWD/charactersheetwindow.h \
    $$PWD/charactersheetmodel.h\
    $$PWD/charactersheet.h \
    $$PWD/charactersheetitem.h \
    $$PWD/field.h\
    $$PWD/tablefield.h\
    $$PWD/csitem.h\
    $$PWD/section.h \
    $$PWD/imagemodel.h \
    $$PWD/sheetwidget.h \
    $$PWD/rolisteamimageprovider.h \
    $$PWD/formula/formulamanager.h \
    $$PWD/formula/parsingtoolformula.h \
    $$PWD/formula/nodes/formulanode.h \
    $$PWD/formula/nodes/startnode.h \
    $$PWD/formula/nodes/operator.h \
    $$PWD/formula/nodes/valuefnode.h \
    $$PWD/formula/nodes/scalaroperatorfnode.h \
    $$PWD/formula/nodes/parenthesesfnode.h

   ## $$PWD/charactersheetbutton.h \
##    $$PWD/charactersheetbutton.cpp \

SOURCES += $$PWD/charactersheetwindow.cpp \
    $$PWD/charactersheetmodel.cpp\
    $$PWD/charactersheet.cpp \
    $$PWD/charactersheetitem.cpp \
    $$PWD/section.cpp \
    $$PWD/imagemodel.cpp \
    $$PWD/field.cpp\
    $$PWD/tablefield.cpp\
    $$PWD/csitem.cpp\
    $$PWD/sheetwidget.cpp \
    $$PWD/rolisteamimageprovider.cpp \
    $$PWD/formula/formulamanager.cpp \
    $$PWD/formula/parsingtoolformula.cpp \
    $$PWD/formula/nodes/formulanode.cpp \
    $$PWD/formula/nodes/startnode.cpp \
    $$PWD/formula/nodes/operator.cpp \
    $$PWD/formula/nodes/valuefnode.cpp \
    $$PWD/formula/nodes/scalaroperatorfnode.cpp \
    $$PWD/formula/nodes/parenthesesfnode.cpp


INCLUDEPATH += $$PWD formula formula/nodes

OTHER_FILES += $$PWD/qml/*.qml

RCSE{
HEADERS -=$$PWD/charactersheetwindow.h
SOURCES -=$$PWD/charactersheetwindow.cpp
}
UNIT_TEST{
HEADERS -=$$PWD/charactersheetwindow.h \
    $$PWD/charactersheetwindow.h \
    $$PWD/charactersheetmodel.h\
    $$PWD/rolisteamimageprovider.h \



SOURCES -=$$PWD/charactersheetwindow.cpp\
    $$PWD/charactersheetmodel.cpp\
    $$PWD/rolisteamimageprovider.cpp \
}

DISTFILES += \
    $$PWD/qml/RLabel.qml \
    $$PWD/qml/Table.qml
