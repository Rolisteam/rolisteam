#include "cleveruri.h"

#include <QDebug>
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
    defineShortName();
}

CleverURI::CleverURI(QString uri,ContentType type)
    : m_uri(uri),m_type(type)
{
    defineShortName();
}

CleverURI::~CleverURI()
{
   // qDebug() << "Destructor of CleverURI";
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
     defineShortName();
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
void CleverURI::defineShortName()
{
    m_shortname = m_uri.mid(m_uri.lastIndexOf('/')+1);
}

const QString& CleverURI::getShortName() const
{
    return m_shortname;
}
void CleverURI::setShortName(QString& name)
{
    //m_shortname = name;
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
  con.defineShortName();
  return is;
}

