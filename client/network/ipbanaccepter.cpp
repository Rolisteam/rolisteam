#include "ipbanaccepter.h"
#include <QDebug>
IpBanAccepter::IpBanAccepter()
{

}

bool IpBanAccepter::isValid(const QMap<QString, QVariant> &data)
{
    qInfo() << "IpBanAccepter";
    QStringList bannedIp = data["IpBan"].toStringList();
    QString currentIp = data["currentIp"].toString();

    // Cut current ip
    bool result = !bannedIp.contains(currentIp);

    auto pos = currentIp.lastIndexOf(':');
    if(pos>-1)
    {
        ++pos;
        auto ipV4 = currentIp.right(currentIp.size()-(pos));
        auto ipV6 = currentIp.left(pos);

        result &= !bannedIp.contains(ipV4);
        result &= !bannedIp.contains(ipV6);

        qInfo() << "bannedIp" << bannedIp << " result" << result ;
    }

    if(nullptr != m_next)
    {
        result &= m_next->isValid(data);
    }
    qInfo() << " result" << result;
    return result;
}
