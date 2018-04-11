#!/bin/sh
#https://developer.github.com/guides/using-ssh-agent-forwarding/
#source ~/.bash_profile
source ~/.ssh/environment
#source ~/.bashrc


QT_DIR="/home/renaud/application/other/Qt/5.9/gcc_64/bin/"

export DISPLAY=:0

cd /home/renaud/application/mine/UnitTestRolisteam/lib/
git clone --recursive https://github.com/Rolisteam/rolisteam.git
git clone https://github.com/Rolisteam/DiceParser.git

rm -rf /home/renaud/application/mine/build-UnitTestRolisteam-Debug/*
mkdir -p /home/renaud/application/mine/build-UnitTestRolisteam-Debug/
cd /home/renaud/application/mine/build-UnitTestRolisteam-Debug
$QT_DIR/qmake -r /home/renaud/application/mine/UnitTestRolisteam/UnitTestRolisteam.pro
make -j 8 > /dev/null

find /home/renaud/application/mine/build-UnitTestRolisteam-Debug -executable -type f -exec bash -c {} \;

cd /home/renaud/application/mine/rolisteam/
doxygen rolisteam.dox  > /dev/null 2>&1
