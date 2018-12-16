include(../tests.pri)
include(../../client/widgets/MRichTextEditor/MRichTextEditor.pri)
include(../../client/undoCmd/undoCmd.pri)
include(../../client/diceparser/diceparser.pri)
include(../../client/session/session.pri)

HEADERS -= ../../../rolisteam/client/diceparser/qmltypesregister.h
SOURCES -= ../../../rolisteam/client/diceparser/qmltypesregister.cpp


HEADERS += $$ROLISTEAMSRC/preferences/preferencesmanager.h \
    $$ROLISTEAMSRC/data/mediacontainer.h \
    $$ROLISTEAMSRC/Features.h \
    $$ROLISTEAMSRC/data/character.h \
    $$ROLISTEAMSRC/data/player.h \
    $$ROLISTEAMSRC/data/person.h \
    $$ROLISTEAMSRC/data/chapter.h \
    $$ROLISTEAMSRC/data/diceshortcut.h \
    $$ROLISTEAMSRC/common/widgets/colorbutton.h \
    $$ROLISTEAMSRC/data/charactervision.h \
    $$ROLISTEAMSRC/userlist/playersList.h \
    $$ROLISTEAMSRC/data/characterstate.h \
    $$ROLISTEAMSRC/network/networkmessage.h \
    $$ROLISTEAMSRC/network/receiveevent.h \
    $$ROLISTEAMSRC/network/networkmessagewriter.h \
    $$ROLISTEAMSRC/network/networkmessagereader.h \
    $$ROLISTEAMSRC/preferences/preferenceslistener.h \
    $$ROLISTEAMSRC/widgets/realslider.h \
    $$ROLISTEAMSRC/widgets/flowlayout.h \
    $$ROLISTEAMSRC/data/cleverurimimedata.h \
    $$ROLISTEAMSRC/widgets/diameterselector.h \
    $$ROLISTEAMSRC/widgets/circledisplayer.h \
    $$ROLISTEAMSRC/userlist/rolisteammimedata.h \
    $$ROLISTEAMSRC/data/resourcesnode.h \
    $$ROLISTEAMSRC/improvedworkspace.h \


SOURCES += \
    tst_vmap.cpp \
    $$ROLISTEAMSRC/data/cleveruri.cpp \
    $$ROLISTEAMSRC/Features.cpp \
    $$ROLISTEAMSRC/data/character.cpp \
    $$ROLISTEAMSRC/data/chapter.cpp \
    $$ROLISTEAMSRC/data/player.cpp \
    $$ROLISTEAMSRC/data/charactervision.cpp \
    $$ROLISTEAMSRC/data/person.cpp \
    $$ROLISTEAMSRC/data/cleverurimimedata.cpp \
    $$ROLISTEAMSRC/userlist/playersList.cpp \
    $$ROLISTEAMSRC/data/characterstate.cpp \
    $$ROLISTEAMSRC/common/widgets/colorbutton.cpp \
    $$ROLISTEAMSRC/data/mediacontainer.cpp \
    $$ROLISTEAMSRC/data/resourcesnode.cpp \
    $$ROLISTEAMSRC/network/receiveevent.cpp \
    $$ROLISTEAMSRC/network/networkmessage.cpp \
    $$ROLISTEAMSRC/network/networkmessagewriter.cpp \
    $$ROLISTEAMSRC/network/networkmessagereader.cpp \
    $$ROLISTEAMSRC/improvedworkspace.cpp \
    $$ROLISTEAMSRC/widgets/realslider.cpp \
    $$ROLISTEAMSRC/widgets/flowlayout.cpp \
    $$ROLISTEAMSRC/widgets/diameterselector.cpp \
    $$ROLISTEAMSRC/widgets/circledisplayer.cpp \
    $$ROLISTEAMSRC/data/diceshortcut.cpp \
    $$ROLISTEAMSRC/userlist/rolisteammimedata.cpp \
    $$ROLISTEAMSRC/preferences/preferenceslistener.cpp \
    $$ROLISTEAMSRC/preferences/preferencesmanager.cpp

TEMPLATE = app
TARGET=tst_vmap
MOC_DIR=trash
OBJECTS_DIR=trash
CONFIG+=testcase


QT += testlib widgets network quick qml

INCLUDEPATH += $$ROLISTEAMSRC/diceparser/
INCLUDEPATH += $$ROLISTEAMSRC/diceparser/node
INCLUDEPATH += $$ROLISTEAMSRC/diceparser/result

include($$ROLISTEAMSRC/vmap/vmap.pri)

ui += $$ROLISTEAMSRC/widgets/
