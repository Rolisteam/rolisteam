#
# Dockerfile for rolisteam-server
#


# Use Debian as base for the build
ARG DEBIAN_VERSION=buster-slim
FROM debian:${DEBIAN_VERSION} AS builder

# Base directory for installed build artifacts.
# Due to limitations of the Docker image build process, this value is
# duplicated in an ARG in the second stage of the build.
#

ARG PREFIX_DIR=/usr/local/rolisteam

# Build arguments
ARG BUILD_DIR=/tmp/rolisteam-docker-BUILD
ARG BUILD_DEPENDENCIES="              \
	qt5-default			\
	qtbase5-dev-tools 		\
	qt5-qmake libqt5network5 	\
	zlib1g-dev build-essential 	\
	git 				\
        ca-certificates			\
	qtmultimedia5-dev 		\
	libqt5core5a 			\
	libqt5gui5 			\
	qttools5-dev-tools 		\
	qtdeclarative5-dev		\
	qtwebengine5-dev		\
	libqt5svg5-dev"		
	
# Bring build environment up to date and install build dependencies
RUN apt-get update                         && \
    apt-get --no-install-recommends install -y $BUILD_DEPENDENCIES && \
    rm -rf /var/lib/apt/lists/*

RUN mkdir $BUILD_DIR && \
    git -C $BUILD_DIR clone --recursive https://invent.kde.org/kde/rolisteam.git && \
    cd $BUILD_DIR/rolisteam && \
    lrelease rolisteam.pro && \
    qmake -r server/server.pro && \
    make && \
    make install && \
    cd ~ && \
    rm -rf $BUILD_DIR && \
    apt-get -y remove *-dev *dev-tools  && \
    apt-get -y autoremove && \
    apt-get -y clean && \
    /usr/local/bin/roliserver -p /opt/default.config 

# Expose the default listener port
EXPOSE 6660

CMD /usr/local/bin/roliserver -c /opt/default.config 


