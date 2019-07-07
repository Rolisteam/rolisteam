include(../tests.pri)

SOURCES += \
    tst_charactersheet.cpp\

TEMPLATE = app
TARGET= tst_charactersheet
MOC_DIR=trash
OBJECTS_DIR=trash
DEFINES+=RCSE
CONFIG+=RCSE

QT += testlib widgets gui
CONFIG += UNIT_TEST testcase

INCLUDEPATH += $$RCSESRC/
INCLUDEPATH += $$RCSESRC/charactersheet/nodes
INCLUDEPATH += $$RCSESRC/charactersheet

HEADERS+=$$RCSESRC/canvas.h \
    $$RCSESRC/controllers/charactercontroller.h \
    $$RCSESRC/controllers/editorcontroller.h \
    $$RCSESRC/controllers/imagecontroller.h \
    $$RCSESRC/controllers/qmlgeneratorcontroller.h \
    $$RCSESRC/data/mockcharacter.h \
    $$RCSESRC/fieldmodel.h \
    $$RCSESRC/borderlisteditor.h \
    $$RCSESRC/qmlhighlighter.h \
    $$RCSESRC/delegate/alignmentdelegate.h \
    $$RCSESRC/codeeditor.h \
    $$RCSESRC/delegate/typedelegate.h \
    $$RCSESRC/canvasfield.h \
    $$RCSESRC/dialog/aboutrcse.h \
    $$RCSESRC/dialog/pdfmanager.h \
    $$RCSESRC/delegate/fontdelegate.h \
    $$RCSESRC/preferences/preferencesdialog.h \
    $$RCSESRC/preferences/preferencesmanager.h \
    $$RCSESRC/dialog/sheetproperties.h \
    $$RCSESRC/itemeditor.h \
    $$RCSESRC/tablecanvasfield.h \
    $$RCSESRC/dialog/columndefinitiondialog.h \
    $$RCSESRC/undo/addfieldcommand.h \
    $$RCSESRC/undo/deletefieldcommand.h \
    $$RCSESRC/undo/movefieldcommand.h \
    $$RCSESRC/undo/setfieldproperties.h \
    $$RCSESRC/undo/addpagecommand.h \
    $$RCSESRC/undo/deletepagecommand.h \
    $$RCSESRC/undo/setbackgroundimage.h \
    $$RCSESRC/undo/addcharactercommand.h \
    $$RCSESRC/undo/deletecharactercommand.h \
    $$RCSESRC/undo/setpropertyonallcharacters.h \
    $$RCSESRC/widgets/codeedit.h \
    $$RCSESRC/delegate/pagedelegate.h \
    $$RCSESRC/dialog/codeeditordialog.h \
    $$RCSESRC/widgets/fieldview.h \
    $$RCSESRC/common/widgets/logpanel.h \
    $$RCSESRC/common/controller/logcontroller.h \
    $$RCSESRC/qmlgeneratorvisitor.h



SOURCES+=$$RCSESRC/controllers/charactercontroller.cpp \
    $$RCSESRC/controllers/editorcontroller.cpp \
    $$RCSESRC/controllers/imagecontroller.cpp \
    $$RCSESRC/controllers/qmlgeneratorcontroller.cpp \
    $$RCSESRC/data/mockcharacter.cpp \
    $$RCSESRC/canvas.cpp \
    $$RCSESRC/fieldmodel.cpp \
    $$RCSESRC/borderlisteditor.cpp \
    $$RCSESRC/qmlhighlighter.cpp \
    $$RCSESRC/delegate/alignmentdelegate.cpp \
    $$RCSESRC/codeeditor.cpp \
    $$RCSESRC/delegate/typedelegate.cpp \
    $$RCSESRC/canvasfield.cpp \
    $$RCSESRC/dialog/aboutrcse.cpp \
    $$RCSESRC/dialog/pdfmanager.cpp \
    $$RCSESRC/delegate/fontdelegate.cpp \
    $$RCSESRC/preferences/preferencesdialog.cpp \
    $$RCSESRC/preferences/preferencesmanager.cpp \
    $$RCSESRC/dialog/sheetproperties.cpp \
    $$RCSESRC/itemeditor.cpp \
    $$RCSESRC/tablecanvasfield.cpp \
    $$RCSESRC/dialog/columndefinitiondialog.cpp \
    $$RCSESRC/undo/addfieldcommand.cpp \
    $$RCSESRC/undo/deletefieldcommand.cpp \
    $$RCSESRC/undo/movefieldcommand.cpp \
    $$RCSESRC/undo/setfieldproperties.cpp \
    $$RCSESRC/undo/addpagecommand.cpp \
    $$RCSESRC/undo/deletepagecommand.cpp \
    $$RCSESRC/undo/setbackgroundimage.cpp \
    $$RCSESRC/undo/addcharactercommand.cpp \
    $$RCSESRC/undo/deletecharactercommand.cpp \
    $$RCSESRC/undo/setpropertyonallcharacters.cpp \
    $$RCSESRC/widgets/codeedit.cpp \
    $$RCSESRC/delegate/pagedelegate.cpp \
    $$RCSESRC/dialog/codeeditordialog.cpp \
    $$RCSESRC/widgets/fieldview.cpp \
    $$RCSESRC/common/widgets/logpanel.cpp \
    $$RCSESRC/common/controller/logcontroller.cpp \
    $$RCSESRC/qmlgeneratorvisitor.cpp

FORMS += $$RCSESRC/dialog/columndefinitiondialog.ui \
    $$RCSESRC/dialog/aboutrcse.ui \
    $$RCSESRC/dialog/pdfmanager.ui \
    $$RCSESRC/preferences/preferencesdialog.ui \
    $$RCSESRC/dialog/sheetproperties.ui \
    $$RCSESRC/dialog/columndefinitiondialog.ui \
    $$RCSESRC/widgets/codeedit.ui \
    $$RCSESRC/dialog/codeeditordialog.ui \
    $$RCSESRC/common/widgets/logpanel.ui

include(../../src/charactersheet/charactersheet.pri)
HEADERS -= ../../RCSESRC/diceparser/qmltypesregister.h
SOURCES -= ../../RCSESRC/diceparser/qmltypesregister.cpp
INCLUDEPATH += $$RCSESRC/charactersheet $$RCSESRC/preferences $$RCSESRC/.
