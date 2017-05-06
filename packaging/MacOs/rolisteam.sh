#! /bin/sh

############################
## Create DMG file
############################

dir="temp"
dmg="dmg/rolisteam.dmg"

rm -rf "$dir"
mkdir "$dir"
mkdir -p "dmg"
cp -R bin/rolisteam.app "$dir"
cp COPYRIGHT "$dir"
cp AUTHORS "$dir"
cp COPYING.txt "$dir"
cp -R resources/doc "$dir"/documentation
hdiutil create -srcfolder "$dir" -volname "rolisteam" "$dmg"
hdiutil internet-enable -yes "$dmg"



