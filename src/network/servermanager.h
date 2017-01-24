#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QObject>

class ServerManager : public QObject
{
    Q_OBJECT
public:
    explicit ServerManager(QObject *parent = 0);

signals:

public slots:
};

#endif // SERVERMANAGER_H