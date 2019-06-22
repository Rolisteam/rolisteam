include(../tests.pri)

TEMPLATE = app
TARGET=unitTest
MOC_DIR=trash
OBJECTS_DIR=trash
CONFIG+=testcase

QT += testlib multimedia


SOURCES += tst_unitmodel.cpp
#          tst_charactermodel.cpp \
#          tst_shortcutmodel.cpp \
#          tst_channelmodel.cpp \
#          tst_characterstatemodel.cpp \
#           tst_sessionitemmodel.cpp \
#           tst_dicebookmarkmodel.cpp \
#           tst_genericmodel.cpp\
#           tst_unitmodel.cpp \

INCLUDEPATH+=$$ROLISTEAMSRC/widgets/gmtoolbox/UnitConvertor/

#include($$ROLISTEAMSRC/widgets/gmtoolbox/UnitConvertor/UnitConvertor.pri)
# unit
SOURCES += $$ROLISTEAMSRC/widgets/gmtoolbox/UnitConvertor/unitmodel.cpp\
        $$ROLISTEAMSRC/widgets/gmtoolbox/UnitConvertor/unit.cpp\

HEADERS += $$ROLISTEAMSRC/widgets/gmtoolbox/UnitConvertor/unitmodel.h\
        $$ROLISTEAMSRC/widgets/gmtoolbox/UnitConvertor/unit.h\


#Others
#SOURCES += $$ROLISTEAMSRC/preferences/preferencesmanager.cpp\
#$$ROLISTEAMSRC/data/cleveruri.cpp \
#    $$ROLISTEAMSRC/data/resourcesnode.cpp \


#HEADERS += $$ROLISTEAMSRC/preferences/preferencesmanager.h\
#$$ROLISTEAMSRC/data/cleveruri.h \
#    $$ROLISTEAMSRC/data/resourcesnode.h \
