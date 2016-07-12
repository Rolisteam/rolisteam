#ifndef HEARTBEATSENDER_H
#define HEARTBEATSENDER_H

#include <QObject>

class heartBeatSender : public QObject
{
    Q_OBJECT
public:
    explicit heartBeatSender(QObject *parent = 0);

signals:

public slots:
};

#endif // HEARTBEATSENDER_H