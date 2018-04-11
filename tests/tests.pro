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
    rolisteamtheme
    #     network \
    #picture \
    #    dataChapter \ #chat MainWindow  picture   map Dice
    #  dataplayer \
    # dataCharacter \
    #networkServer \

OBJECTS_DIR = obj
OTHER_FILES = UnitTestRolisteam.pri





