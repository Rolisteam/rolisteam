#!/bin/sh


GIT_ACCOUNT=renaudg


echo "##################################"
echo "# Add remote to charactersheet   #"
echo "##################################"

cd src/libraries/charactersheet
git remote add perso git@invent.kde.org:$GIT_ACCOUNT/rcharactersheet.git
cd -


echo "##################################"
echo "# Add remote to common           #"
echo "##################################"
cd src/libraries/common
git remote add perso git@invent.kde.org:$GIT_ACCOUNT/rolisteam-common.git
cd -


echo "##################################"
echo "# Add remote to diceparser       #"
echo "##################################"
cd s3yyrc/libraries/diceparser 
git remote add perso git@invent.kde.org:$GIT_ACCOUNT/rolisteam-diceparser.git
cd -

echo "##################################"
echo "# Add remote to gmtools          #"
echo "##################################"
cd src/libraries/rwidget/gmtoolbox
git remote add perso git@invent.kde.org:$GIT_ACCOUNT/rolisteam-plugins.git
cd -
