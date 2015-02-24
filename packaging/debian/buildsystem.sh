#!/bin/sh
VERSION=1.5.2

if [  $# -gt 0 ]
then
	for i in "$@"; do
        if [ "$i" = "clean" ]
		then
		    	rm -rf rolisteam
		fi
        if [ "$i" = "tarball" ]
		then
		    mkdir rolisteam
            cp ./rolisteam.desktop rolisteam/
            cp ./changelog rolisteam/
            cd rolisteam
            svn export https://rolisteam.googlecode.com/svn/branches/1.0.0/rolisteam/ rolisteam-$VERSION
            cp ./rolisteam.desktop rolisteam-$VERSION/
            cp ./changelog rolisteam-$VERSION/
            tar -czf rolisteam-$VERSION.tar.gz rolisteam-$VERSION
            zip -r rolisteam-$VERSION.zip rolisteam-$VERSION
            
		fi
		if [ "$i" = "setup" ]
		then
		    sudo apt-get install libphonon-dev libqt4-dev build-essential cdbs debhelper wdiff  devscripts dh-make dpatch vim subversion
		fi
		if [ "$i" = "build" ]
		then
			mkdir rolisteam
			cp ./rolisteam.desktop rolisteam/
			cd rolisteam
			svn export https://rolisteam.googlecode.com/svn/branches/1.0.0/rolisteam/ rolisteam-$VERSION
			mv ./rolisteam.desktop rolisteam-$VERSION
			cd rolisteam-$VERSION
			svn export http://rolisteam.googlecode.com/svn/trunk/packaging/debian debian
			cd ..
			cp ../changelog rolisteam-$VERSION/debian/
			#find . -name ".svn" -exec rm -rf {} \;  2> /dev/null
			cd rolisteam-$VERSION
			cd traduction
			lrelease rolisteam_*.ts 
			cd ..
			#dch -i
			dpkg-buildpackage -rfakeroot
		fi
	done
fi
