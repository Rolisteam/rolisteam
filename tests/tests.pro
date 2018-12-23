TEMPLATE = subdirs

DEFINES =DSTDIR=$$SRCDIR
SUBDIRS = \
    Dice \
    dataCleverURI \
    preferences \
    colorButton\
    paletteModel\
    chat \
    charactersheet \
    rolisteamtheme \
    dataplayer \
    picture \
    session \
    dataCharacter \
    fieldModelTest \
    sendToServer \
    dataChapter \
    vmap \
    widgets \
    network \
    map \
    musicmodel \
    channelmodel\
    characterstatemodel \


#    dicebookmarkmodel   \
#    unitmodel           \
#    shortcutmodel       \
#    genericmodel        \
#    charactermodel      \


    #    MainWindow  picture
    #networkServer \

OBJECTS_DIR = obj
OTHER_FILES = tests.pri



