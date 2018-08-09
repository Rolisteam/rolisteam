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
    sendToServer \
    dataChapter
    #     network \
    #    MainWindow  picture   map Dice
    #networkServer \

OBJECTS_DIR = obj
OTHER_FILES = tests.pri



