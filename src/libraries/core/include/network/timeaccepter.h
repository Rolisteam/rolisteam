#ifndef TIMEACCEPTER_H
#define TIMEACCEPTER_H

#include "connectionaccepter.h"
#include "network_global.h"

class NETWORK_EXPORT TimeAccepter : public ConnectionAccepter
{
public:
    TimeAccepter();

    virtual bool isValid(const QMap<QString, QVariant>& data) const override;
};

#endif // TIMEACCEPTER_H
