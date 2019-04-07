#include "timeaccepter.h"
#include <QDateTime>
#include <QDebug>
#include <QTime>

TimeAccepter::TimeAccepter() {}

bool TimeAccepter::isValid(const QMap<QString, QVariant>& data) const
{
    QTime time(QTime::currentTime());
    const QString format= QStringLiteral("hh:mm");

    QTime start= QTime::fromString(data["TimeStart"].toString(), format);
    QTime end= QTime::fromString(data["TimeEnd"].toString(), format);
    QDateTime dateStart;
    dateStart.setTime(start);
    QDateTime dateEnd;
    dateEnd.setTime(end);

    QDateTime dateTime;
    dateTime.setTime(time);

    if(dateEnd < dateStart)
        dateEnd= dateEnd.addDays(1);

    bool result= true;
    if(start.isValid() && end.isValid() && ((dateTime < dateStart) || (dateTime > dateEnd)))
    {
        result= false;
        qInfo() << QStringLiteral("Connection out of time slot. Connection refused");
    }
    return result;
}
