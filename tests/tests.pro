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
    vmap

    #     network \
    #    MainWindow  picture   map
    #networkServer \

OBJECTS_DIR = obj
OTHER_FILES = tests.pri



