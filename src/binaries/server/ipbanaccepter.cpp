#include "ipbanaccepter.h"

IpBanAccepter::IpBanAccepter() {}

bool IpBanAccepter::isValid(const QMap<QString, QVariant>& data) const
{
    QList<QVariant> bannedIp= data["bannedIp"].toList();
    QString currentIp= data["clientIp"].toString();

    bool result= true;

    for(const QVariant &ip : bannedIp)
    {
        if(currentIp == ip.toString())
        {
            result= false;
        }
    }
    return result;
}
