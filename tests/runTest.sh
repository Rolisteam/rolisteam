#!/bin/sh


export DISPLAY=:1

echo "Starting test compilation"
DEST=test_directory

rm -rf $DEST
mkdir -p $DEST
cd $DEST
qmake -r ../tests.pro
make -j8 > /dev/null

find . -executable -type f -exec bash -c {} \;

echo "end of test"
#doxygen rolisteam.dox  > /dev/null 2>&1
