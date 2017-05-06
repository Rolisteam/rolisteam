#!/bin/sh

############################################
# Rolisteam 
# Script to deploy libz into rolisteam.
#
###########################################

echo 'Display dependancies'
otool -L rolisteam.app/Contents/MacOS/rolisteam



echo 'Create directory'
mkdir rolisteam.app/Contents/Frameworks/libz.framework/

echo 'Copy lib binary'
cp /usr/lib/libz.1.dylib rolisteam.app/Contents/Frameworks/libz.framework/

echo 'Rewrite path to the lib'
install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.framework/libz.1.dylib rolisteam.app/Contents/MacOS/rolisteam
