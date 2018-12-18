#include "timeaccepter.h"
#include "QTime"
#include <QDebug>
TimeAccepter::TimeAccepter()
{

}

bool TimeAccepter::isValid(const QMap<QString, QVariant> &data)
{
    QTime time(QTime::currentTime());
    const QString format = QStringLiteral("hh:mm");

    QTime start= QTime::fromString(data["TimeStart"].toString(),format);
    QTime end= QTime::fromString(data["TimeEnd"].toString(),format);
    bool result = true;
    if(start.isValid()&&
       end.isValid()&&
       ((time<start)||(time>end)))
    {
        result = false;
        qInfo() << QStringLiteral("Connection out of time slot. Connection refused");
    }
    if(nullptr != m_next)
    {
        result &= m_next->isValid(data);
    }
    return result;
}
