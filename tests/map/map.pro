include(../tests.pri)

TEMPLATE = app
TARGET=mapTest
MOC_DIR=trash
OBJECTS_DIR=trash
CONFIG+=testcase


QT += testlib widgets network


HEADERS += $$ROLISTEAMSRC/map/charactertoken.h \
    $$ROLISTEAMSRC/data/mediacontainer.h \
        $$ROLISTEAMSRC/map/map.h \
    $$ROLISTEAMSRC/Features.h \
        $$ROLISTEAMSRC/map/mapframe.h \
    $$ROLISTEAMSRC/map/newemptymapdialog.h \
$$ROLISTEAMSRC/map/mapwizzard.h \
    $$ROLISTEAMSRC/network/receiveevent.h \
    $$ROLISTEAMSRC/network/networkmessagewriter.h \
    $$ROLISTEAMSRC/network/networkmessagereader.h \
    $$ROLISTEAMSRC/userlist/playersList.h \
    $$ROLISTEAMSRC/data/character.h \
    $$ROLISTEAMSRC/data/player.h \
$$ROLISTEAMSRC/network/receiveevent.h \
    $$ROLISTEAMSRC/data/cleveruri.h \
    $$ROLISTEAMSRC/data/person.h \
    $$ROLISTEAMSRC/data/resourcesnode.h \
$$ROLISTEAMSRC/preferences/preferencesmanager.h \
    $$ROLISTEAMSRC/widgets/colorselector.h \
    $$ROLISTEAMSRC/widgets/flowlayout.h \
    $$ROLISTEAMSRC/data/characterstate.h \
$$ROLISTEAMSRC/common/widgets/colorbutton.h \


SOURCES += tst_map.cpp \
    $$ROLISTEAMSRC/data/mediacontainer.cpp \
    $$ROLISTEAMSRC/Features.cpp \
        $$ROLISTEAMSRC/map/charactertoken.cpp \
        $$ROLISTEAMSRC/map/map.cpp \
        $$ROLISTEAMSRC/map/mapframe.cpp \
$$ROLISTEAMSRC/data/cleveruri.cpp \
    $$ROLISTEAMSRC/map/newemptymapdialog.cpp \
$$ROLISTEAMSRC/map/mapwizzard.cpp \
    $$ROLISTEAMSRC/network/receiveevent.cpp \
    $$ROLISTEAMSRC/network/networkmessage.cpp \
    $$ROLISTEAMSRC/network/networkmessagewriter.cpp \
    $$ROLISTEAMSRC/network/networkmessagereader.cpp \
    $$ROLISTEAMSRC/userlist/playersList.cpp \
    $$ROLISTEAMSRC/data/player.cpp \
    $$ROLISTEAMSRC/data/characterstate.cpp \
    $$ROLISTEAMSRC/data/resourcesnode.cpp \
    $$ROLISTEAMSRC/data/character.cpp \
    $$ROLISTEAMSRC/data/person.cpp \
$$ROLISTEAMSRC/widgets/flowlayout.cpp \
    $$ROLISTEAMSRC/widgets/colorselector.cpp \
$$ROLISTEAMSRC/preferences/preferencesmanager.cpp \
$$ROLISTEAMSRC/common/widgets/colorbutton.cpp \

FORMS += $$ROLISTEAMSRC/map/mapwizzard.ui \
      $$ROLISTEAMSRC/map/newemptymapdialog.ui \
