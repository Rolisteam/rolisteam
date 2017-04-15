#include "ipbanaccepter.h"
#include <QDebug>
IpBanAccepter::IpBanAccepter()
{

}

bool IpBanAccepter::isValid(const QMap<QString, QVariant> &data)
{
    qInfo() << "IpBanAccepter";
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
    qInfo() << "bannedIp" << bannedIp << " result" << result;

    if(nullptr != m_next)
    {
        result &= m_next->isValid(data);
    }
    qInfo() << " result" << result;
    return result;
}
