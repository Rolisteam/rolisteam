#include "cleveruri.h"
#include <QString>
#include <QDebug>
/////////////////
// CleverUri
/////////////////
QString CleverURI::m_textIcon=QString(":/resources/icons/notes.png");
QString CleverURI::m_mapIcon=QString(":/resources/icons/map.png");
QString CleverURI::m_pictureIcon=QString(":/resources/icons/image.png");
QString CleverURI::m_charactersheetIcon=QString(":/resources/icons/treeview.png");
QString CleverURI::m_scenarioIcon=QString(":/resources/icons/scenario.png");
QString CleverURI::m_tchatIcon=QString(":/resources/icons/scenario.png");

CleverURI::CleverURI()
{

}

CleverURI::CleverURI(const CleverURI & mp)
{
    m_type=mp.getType();
    m_uri=mp.getUri();
    defineShortName();
}
QString& CleverURI::getIcon(ContentType type)
{
    switch(type)
    {
    case CleverURI::CHARACTERSHEET:
        return m_charactersheetIcon;
        break;
    case CleverURI::PICTURE:
        return m_pictureIcon;
        break;
    case CleverURI::MAP:
         return m_mapIcon;
        break;
    case CleverURI::TCHAT:
        return m_tchatIcon;
        break;
    case CleverURI::TEXT:
        return m_textIcon;
        break;
    case CleverURI::SCENARIO:
        return m_scenarioIcon;
        break;
    }
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

