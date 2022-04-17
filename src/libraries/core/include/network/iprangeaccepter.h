#ifndef IPRANGEACCTEPTER_H
#define IPRANGEACCTEPTER_H

#include "connectionaccepter.h"
#include "network_global.h"

class NETWORK_EXPORT IpRangeAccepter : public ConnectionAccepter
{
public:
    IpRangeAccepter();

    virtual bool isValid(const QMap<QString, QVariant>& data) const override;
};

#endif // IPRANGEACCTEPTER_H
