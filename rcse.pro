#-------------------------------------------------
#
# Project created by QtCreator 2016-01-04T16:16:31
#
#-------------------------------------------------

QT       += core gui quickwidgets quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bin/rcse
TEMPLATE = app

QMAKE_CXXFLAGS = -std=c++11

DEFINES+=RCSE
CONFIG+=RCSE

isEmpty(PREFIX) {
 PREFIX = /usr/bin
}


## Translation
TRANSLATIONS =  translations/rcse_fr.ts \
                translations/rcse.ts \
                translations/rcse_de.ts \
                translations/rcse_pt_BR.ts \
                translations/rcse_hu_HU.ts \
                translations/rcse_tr.ts \

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}
isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}
updateTrans.input = rcse.pro
updateTrans.output= ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.ts
updateTrans.command= ${QMAKE_LRELEASE} rcse.pro

unix:!macx{
# linux only

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateTrans updateqm
PRE_TARGETDEPS += compiler_updateqm_make_all
## End of Translation

##Installation
target.path = $$PREFIX/

}
INSTALLS += target

include(charactersheet/charactersheet.pri)

INCLUDEPATH += charactersheet

SOURCES += main.cpp\
        mainwindow.cpp \
    canvas.cpp \
    fieldmodel.cpp \
    borderlisteditor.cpp \
    qmlhighlighter.cpp \
    charactermodel.cpp \
    alignmentdelegate.cpp \
    codeeditor.cpp \
    typedelegate.cpp \
    canvasfield.cpp

HEADERS  += mainwindow.h \
    canvas.h \
    fieldmodel.h \
    borderlisteditor.h \
    qmlhighlighter.h \
    charactermodel.h \
    alignmentdelegate.h \
    codeeditor.h \
    typedelegate.h \
    canvasfield.h

FORMS    += mainwindow.ui

DISTFILES += \
    charactersheet/qml/*.qml \
    resources/icons/photo.png

RESOURCES += \
    qmlfile.qrc

ICON = resources/logo/rcse.icns
win32 {
RC_FILE = "resources/logo/rcse.rc"
OTHER_FILES +=resources/logo/rcse.rc
}
