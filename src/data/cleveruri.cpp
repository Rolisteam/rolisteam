#include "cleveruri.h"

/////////////////
// CleverUri
/////////////////

CleverURI::CleverURI()
{

}

CleverURI::CleverURI(const CleverURI & mp)
{
    m_type=mp.getType();
    m_uri=mp.getUri();
}

CleverURI::CleverURI(QString uri,ContentType type)
    : m_uri(uri),m_type(type)
{

}

CleverURI::~CleverURI()
{

}
bool CleverURI::operator==(const CleverURI& uri) const
{
    if((uri.getUri()==getUri())&&(uri.getType()==getType()))
        return true;
    return false;
}

void CleverURI::setUri(QString& uri)
{
 m_uri=uri;
}

void CleverURI::setType(int type)
{
    m_type=type;
}

const QString& CleverURI::getUri() const
{
    return m_uri;
}

int CleverURI::getType() const
{
    return m_type;
}
bool CleverURI::hasChildren() const
{
    return false;
}
const QString& CleverURI::getShortName() const
{
    return m_uri;
}
QDataStream& operator<<(QDataStream& out, const CleverURI& con)
{
  out << con.getUri();
  out << con.getType();
  return out;
}

QDataStream& operator>>(QDataStream& is,CleverURI& con)
{
  is >>(con.m_uri);
  is >>(con.m_type);
  return is;
}

