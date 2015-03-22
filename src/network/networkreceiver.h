#ifndef NETWORKRECEIVER_H
#define NETWORKRECEIVER_H

#include "network/networkmessagereader.h"

class NetWorkReceiver
{
public:
    enum SendType { NONE, ALL,AllExceptMe};
    virtual NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) = 0;
};

#endif // NETWORKRECEIVER_H
