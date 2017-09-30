#include "iprangeaccepter.h"
#include <QHostAddress>

IpRangeAccepter::IpRangeAccepter()
{

}

bool IpRangeAccepter::isValid(const QMap<QString, QVariant> &data)
{
    bool result = true;
    if(!data["rangeIp"].toString().isEmpty())
    {
        QHostAddress ip(data["currentIp"].toString());
        QPair<QHostAddress,int> subNet = QHostAddress::parseSubnet(data["rangeIp"].toString());
        if(!ip.isInSubnet(subNet.first,subNet.second))
        {
            result = false;
        }
    }
    //qInfo() << "result" << result;
    if(nullptr != m_next)
    {
        result &= m_next->isValid(data);
    }
    //qInfo() << "result" << result;
    return result;
}
