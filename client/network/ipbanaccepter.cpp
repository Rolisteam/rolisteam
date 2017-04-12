#include "ipbanaccepter.h"

IpBanAccepter::IpBanAccepter()
{

}

bool IpBanAccepter::isValid(const QMap<QString, QVariant> &data)
{
    QList<QVariant> bannedIp = data["bannedIp"].toList();
    QString currentIp = data["clientIp"].toString();

    bool result=true;

    for(QVariant ip : bannedIp)
    {
        if(currentIp == ip.toString())
        {
            result = false;
        }
    }

    if(nullptr != m_next)
    {
        result &= m_next->isValid(data);
    }

    return result;
}
