#ifndef NETWORKRECEIVER_H
#define NETWORKRECEIVER_H

#include "network/networkmessagereader.h"

class NetWorkReceiver
{
public:
    virtual void processMessage(NetworkMessageReader* msg) = 0;
};

#endif // NETWORKRECEIVER_H
