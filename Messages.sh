#! /bin/bash

#Extract strings
$EXTRACT_TR_STRINGS `find . -name \*.cpp -o -name \*.h -o -name \*.qml  -o -name \*.ui | grep -v "libraries/diceparser"` -o $podir/rolisteam_qt.pot
