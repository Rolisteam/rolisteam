#ifndef MEDIACONTAINER_H
#define MEDIACONTAINER_H

#include <QString>

class MediaContainer
{
public:
    MediaContainer();


    void setLocalPlayerId(QString id);
    QString getLocalPlayerId();
protected:
    QString m_localPlayerId;


};

#endif // MEDIACONTAINER_H
