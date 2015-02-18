#include "chapter.h"

Chapter::Chapter()
{
}
Chapter::Chapter(const Chapter& m)
{
    m_name=m.m_name;
}

Chapter::~Chapter()
{

}

bool Chapter::hasChildren() const
{
    return (m_ressoucelist.size()>0)?true:false;
}
const QString& Chapter::getShortName() const
{
    return m_name;
}
void Chapter::setName(QString& p )
{
 m_name =p;
}
void Chapter::addResource(CleverURI* cluri)
{
    m_ressoucelist.append(*cluri);
}
QList<CleverURI>& Chapter::getResourceList()
{
    return m_ressoucelist;
}

QList<Chapter>& Chapter::getChapterList()
{
    return m_chapterlist;
}

QDataStream& operator<<(QDataStream& os,const Chapter& chap)
{
    os << chap.m_name;
    os << chap.m_ressoucelist;
    return os;
}

QDataStream& operator>>(QDataStream& is,Chapter& chap)
{
    is >> chap.m_name;
    is >> chap.m_ressoucelist;
    return is;

}
