TEMPLATE = subdirs

DEFINES =DSTDIR=$$SRCDIR
SUBDIRS = \
    dataCleverURI \
    preferences \
    colorButton\
    paletteModel\
    chat \
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
    unitmodel           \

# inside charactersheet now
#    charactersheetmodel      \
#    charactersheet \
    #Dice \

# TODO
#    dicebookmarkmodel   \
#    shortcutmodel       \
#    MainWindow
#   networkServer \

OBJECTS_DIR = obj
OTHER_FILES = tests.pri



