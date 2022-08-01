#! /bin/bash

$EXTRACT_TR_STRINGS `find . -name \*.cpp -o -name \*.h -o -name \*.qml | grep -v "libraries/diceparser"` -o $podir/rolisteam_qt.pot
