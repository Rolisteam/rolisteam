#include "mediacontainer.h"

MediaContainer::MediaContainer()
{
}
void MediaContainer::setLocalPlayerId(QString id)
{
    m_localPlayerId = id;
}

QString MediaContainer::getLocalPlayerId()
{
    return m_localPlayerId;
}
