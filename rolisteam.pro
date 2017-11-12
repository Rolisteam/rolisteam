##################################################
## Rolisteam Project file
## @author Renaud Guezennec
##################################################

## Generic parameter
CONFIG +=  -console
#-console
#

# Version
DEFINES += VERSION_MAJOR=1 VERSION_MIDDLE=9 VERSION_MINOR=0

TEMPLATE = subdirs
SUBDIRS = client server



MOC_DIR = bin
OBJECTS_DIR = bin

UI_DIR = bin

RESOURCES += rolisteam.qrc
