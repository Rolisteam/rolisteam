#ifndef ROLISTEAMDAEMON_H
#define ROLISTEAMDAEMON_H

#include <QObject>

class RolisteamDaemon : public QObject
{
    Q_OBJECT
public:
    explicit RolisteamDaemon(QObject *parent = 0);

signals:

public slots:
};

#endif // ROLISTEAMDAEMON_H