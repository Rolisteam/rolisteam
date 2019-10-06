#ifndef PASSWORDACCEPTER_H
#define PASSWORDACCEPTER_H

#include "connectionaccepter.h"
class PasswordAccepter : public ConnectionAccepter
{
public:
    enum Level
    {
        Connection,
        Admin,
        Channel
    };
    explicit PasswordAccepter(PasswordAccepter::Level level= Connection);

    virtual bool isValid(const QMap<QString, QVariant>& data) const override;

private:
    PasswordAccepter::Level m_currentLevel;
};

#endif // PASSWORDACCEPTER_H
