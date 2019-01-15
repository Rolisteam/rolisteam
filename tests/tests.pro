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
    dicealiasmodel \
    genericmodel   \
    charactersheetmodel      \



#    dicebookmarkmodel   \
#    unitmodel           \
#    shortcutmodel       \


    #    MainWindow  picture
    #networkServer \

OBJECTS_DIR = obj
OTHER_FILES = tests.pri



