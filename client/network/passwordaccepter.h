#ifndef PASSWORDACCEPTER_H
#define PASSWORDACCEPTER_H

#include "connectionaccepter.h"
class PasswordAccepter : public ConnectionAccepter
{
public:
    PasswordAccepter();

    virtual bool isValid(const QMap<QString,QVariant>& data);
};

#endif // PASSWORDACCEPTER_H
