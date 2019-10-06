#ifndef IPRANGEACCTEPTER_H
#define IPRANGEACCTEPTER_H

#include "connectionaccepter.h"
class IpRangeAccepter : public ConnectionAccepter
{
public:
    IpRangeAccepter();

    virtual bool isValid(const QMap<QString, QVariant>& data) const override;
};

#endif // IPRANGEACCTEPTER_H
