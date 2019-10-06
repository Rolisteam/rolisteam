#ifndef IPBANACCEPTER_H
#define IPBANACCEPTER_H

#include "connectionaccepter.h"
class IpBanAccepter : public ConnectionAccepter
{
public:
    IpBanAccepter();

    bool isValid(const QMap<QString, QVariant>& data) const override;
};

#endif // IPBANACCEPTER_H
