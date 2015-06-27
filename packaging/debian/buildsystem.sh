#!/bin/sh

#############################################
## Script for submitting rolisteam into ppa
## There is two ppa: unstable(dev) and stable.
#############################################
VERSION=1.7.0

GIT_PROJECT="git@github.com:Rolisteam/rolisteam.git"
DEV=1


rm -rf rolisteam-$VERSION

function makeTarBall()
{
    git clone --recursive $GIT_PROJECT
    cp ./rolisteam/packaging/rolisteam.desktop ./rolisteam/
    cp ./rolisteam/packaging/debian/changelog ./rolisteam/
    #Rename the dir
    mv ./rolisteam ./rolisteam-$VERSION
    rm -rf ./rolisteam/packaging
    find . -name ".git" -exec rm -rf {} \;
    tar -czf rolisteam-$VERSION.tar.gz rolisteam-$VERSION
    zip -r rolisteam-$VERSION.zip rolisteam-$VERSION
}

function build()
{
    git clone --recursive $GIT_PROJECT
    mv rolisteam rolisteam-$VERSION
    cp ./rolisteam-$VERSION/packaging/rolisteam.desktop ./rolisteam-$VERSION/
    cp -R ./rolisteam-$VERSION/packaging/debian ./rolisteam-$VERSION/
    cp ./rolisteam-$VERSION/packaging/debian/changelog ./rolisteam-$VERSION/
    cd rolisteam-$VERSION
    id=`head -n 1 changelog | awk '{print $2}' | awk -F ')' '{print $1}' | awk -F 'ubuntu' '{print $2}'`
    lrelease rolisteam.pro 
    rm -rf packaging
    echo "Would you like to create new version in changelog file? [N/y]"
    read value
    if [ $a = "y" ]
    then
        dch -i
    fi
    #dpkg-buildpackage -rfakeroot
    debuild -S -sa
    cd ..
    if [ $DEV -eq 1 ]
    then 
        dput -f ppa:rolisteam/rolisteamdev rolisteam_"$VERSION"ubuntu"$id"_source.changes
    else
        dput -f ppa:rolisteam/ppa rolisteam_"$VERSION"ubuntu"$id"_source.changes
    fi

}


if [  $# -gt 0 ]
then
	for i in "$@"; do
		if [ "$i" = "clean" ]
		then
		   rm -rf rolisteam
		fi
		if [ "$i" = "tarball" ]
		then
		   makeTarBall
		fi
		if [ "$i" = "setup" ]
		then
		    sudo apt-get install build-essential cdbs debhelper wdiff  devscripts dh-make dpatch vim git
		fi
		if [ "$i" = "build" ]
		then
			build
		fi
	done
fi
