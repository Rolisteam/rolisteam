HEADERS += $$PWD/charactersheetwindow.h \
    $$PWD/charactersheetmodel.h\
    $$PWD/charactersheet.h \
    $$PWD/charactersheetitem.h \
    $$PWD/charactersheetbutton.h \
    $$PWD/field.h\
    $$PWD/csitem.h\
    $$PWD/section.h \
    $$PWD/rolisteamimageprovider.h


SOURCES += $$PWD/charactersheetwindow.cpp \
    $$PWD/charactersheetmodel.cpp\
    $$PWD/charactersheet.cpp \
    $$PWD/charactersheetbutton.cpp \
    $$PWD/charactersheetitem.cpp \
    $$PWD/section.cpp \
    $$PWD/field.cpp\
    $$PWD/csitem.cpp\
    $$PWD/rolisteamimageprovider.cpp


RCSE{
HEADERS -=$$PWD/charactersheetwindow.h
SOURCES -=$$PWD/charactersheetwindow.cpp
}
