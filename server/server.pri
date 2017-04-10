SOURCES += main.cpp \
           ../client/network/rolisteamdaemon.cpp \
            ../client/network/channelmodel.cpp \
            ../client/network/tcpclient.cpp \
           ../client/network/servermanager.cpp \
    $$PWD/connectionaccepter.cpp \
    $$PWD/passwordaccepter.cpp \
    $$PWD/iprangeacctepter.cpp \
    $$PWD/ipbanaccepter.cpp \
    $$PWD/timeaccepter.cpp

HEADERS += ../client/network/rolisteamdaemon.h \
            ../client/network/channelmodel.h \
            ../client/network/tcpclient.h \
            ../client/network/servermanager.h \
    $$PWD/connectionaccepter.h \
    $$PWD/passwordaccepter.h \
    $$PWD/iprangeacctepter.h \
    $$PWD/ipbanaccepter.h \
    $$PWD/timeaccepter.h
