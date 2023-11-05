#! /bin/bash

#Make sure UI Files are translated as well
$EXTRACTRC `find . -name \*.rc -o -name \*.ui` >> rc.cpp

#Extract strings
$EXTRACT_TR_STRINGS `find . -name \*.cpp -o -name \*.h -o -name \*.qml | grep -v "libraries/diceparser"` -o $podir/rolisteam_qt.pot
