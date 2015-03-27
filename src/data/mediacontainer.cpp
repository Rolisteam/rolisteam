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
void MediaContainer::setCleverUri(CleverURI* uri)
{
	m_uri = uri;
}
QString MediaContainer::getTitle() const
{
	return m_title;
}
void MediaContainer::openMedia()
{

}
