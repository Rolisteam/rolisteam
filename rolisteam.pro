##################################################
## Rolisteam Project file
## @author Renaud Guezennec
##################################################

## Generic parameter
CONFIG +=  -console
QT += widgets printsupport multimedia quick qml quickwidgets
#-console
#

# Version
DEFINES += VERSION_MAJOR=1 VERSION_MIDDLE=9 VERSION_MINOR=0

TEMPLATE = subdirs
SUBDIRS = client server


#DEFINES += DEBUG_MODE
CONFIG += c++11

MOC_DIR = bin
OBJECTS_DIR = bin

UI_DIR = bin

RESOURCES += rolisteam.qrc
