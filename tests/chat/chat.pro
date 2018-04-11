include(../tests.pri)

TARGET = tst_chat
CONFIG   += console
CONFIG   -= app_bundle



QT += widgets gui printsupport xml qml

TEMPLATE = app

HEADERS += $$ROLISTEAMSRC/chat/improvedtextedit.h \
    $$ROLISTEAMSRC/chat/chatwindow.h \
    $$ROLISTEAMSRC/preferences/preferencesmanager.h\
    $$ROLISTEAMSRC/data/person.h\
    $$ROLISTEAMSRC/data/player.h \
    $$ROLISTEAMSRC/data/character.h \
    $$ROLISTEAMSRC/data/diceshortcut.h \
    $$ROLISTEAMSRC/data/characterstate.h \
    $$ROLISTEAMSRC/network/networkmessage.h\
    $$ROLISTEAMSRC/widgets/gmtoolbox/DiceBookMark/dicebookmarkwidget.h \
    $$ROLISTEAMSRC/chat/chatbrowser.h\
    $$ROLISTEAMSRC/chat/chat.h\
    $$ROLISTEAMSRC/network/networkmessagereader.h\
    $$ROLISTEAMSRC/network/networkmessagewriter.h\
    $$ROLISTEAMSRC/network/networkmessage.h\
    $$ROLISTEAMSRC/network/receiveevent.h\
    $$ROLISTEAMSRC/userlist/rolisteammimedata.h \
    $$ROLISTEAMSRC/widgets/gmtoolbox/DiceBookMark/dicebookmarkmodel.h \
    $$ROLISTEAMSRC/data/cleveruri.h\
    $$ROLISTEAMSRC/data/resourcesnode.h\
    $$ROLISTEAMSRC/localpersonmodel.h\
    $$ROLISTEAMSRC/Features.h\
    $$ROLISTEAMSRC/userlist/playersList.h\
    $$ROLISTEAMSRC/userlist/playerslistproxy.h\
    networklink.h

#    $$ROLISTEAMSRC/charactersheet/charactersheet.h\
SOURCES += \
    tst_chat.cpp \
    $$ROLISTEAMSRC/preferences/preferencesmanager.cpp\
    $$ROLISTEAMSRC/chat/chatwindow.cpp \
    $$ROLISTEAMSRC/chat/improvedtextedit.cpp \
    $$ROLISTEAMSRC/data/person.cpp\
    $$ROLISTEAMSRC/data/player.cpp\
    $$ROLISTEAMSRC/data/diceshortcut.cpp \
    $$ROLISTEAMSRC/data/character.cpp \
    $$ROLISTEAMSRC/data/characterstate.cpp \
    $$ROLISTEAMSRC/chat/chatbrowser.cpp\
    $$ROLISTEAMSRC/chat/chat.cpp\
    $$ROLISTEAMSRC/widgets/gmtoolbox/DiceBookMark/dicebookmarkwidget.cpp \
    $$ROLISTEAMSRC/widgets/gmtoolbox/DiceBookMark/dicebookmarkmodel.cpp \
    $$ROLISTEAMSRC/network/networkmessagereader.cpp\
    $$ROLISTEAMSRC/network/networkmessagewriter.cpp\
    $$ROLISTEAMSRC/network/receiveevent.cpp\
    $$ROLISTEAMSRC/network/networkmessage.cpp\
    $$ROLISTEAMSRC/data/cleveruri.cpp\
    $$ROLISTEAMSRC/data/resourcesnode.cpp\
    $$ROLISTEAMSRC/userlist/rolisteammimedata.cpp \
    $$ROLISTEAMSRC/localpersonmodel.cpp\
    $$ROLISTEAMSRC/Features.cpp\
    $$ROLISTEAMSRC/userlist/playersList.cpp\
    $$ROLISTEAMSRC/userlist/playerslistproxy.cpp\
    networklink.cpp

#    $$ROLISTEAMSRC/charactersheet/charactersheet.cpp\
#    $$ROLISTEAMSRC/network/networklink.cpp\     $$ROLISTEAMSRC/network/networklink.h\





INCLUDEPATH += $$ROLISTEAMSRC/chat/ $$ROLISTEAMSRC/ $$ROLISTEAMSRC/widgets $$ROLISTEAMSRC/odt/format_OO_oasis_ $$ROLISTEAMSRC/odt $$ROLISTEAMSRC/network ../ $$ROLISTEAMSRC/preferences
INCLUDEPATH += $$ROLISTEAMSRC/diceparser/
INCLUDEPATH += $$ROLISTEAMSRC/diceparser/node
INCLUDEPATH += $$ROLISTEAMSRC/diceparser/result

FORMS += $$ROLISTEAMSRC/mainwindow.ui $$ROLISTEAMSRC/widgets/gmtoolbox/DiceBookMark/dicebookmarkwidget.ui

include($$ROLISTEAMSRC/diceparser/diceparser.pri)
