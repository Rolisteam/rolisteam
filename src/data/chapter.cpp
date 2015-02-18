#include "chapter.h"

Chapter::Chapter()
{
}
Chapter::Chapter(const Chapter& m)
{
    m_name=m.m_name;
    m_ressoucelist=m.m_ressoucelist;
    m_chapterlist=m.m_chapterlist;
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
void Chapter::addResource(CleverURI* cluri)
{
    m_ressoucelist.append(*cluri);
}
Chapter* Chapter::addChapter(QString& name)
{
    Chapter* t = new Chapter();
    t->setShortName(name);
    m_chapterlist.append(*t);
    return t;
}

void Chapter::setShortName(QString& name)
{
    m_name = name;
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
    os << chap.m_chapterlist;
    return os;
}

QDataStream& operator>>(QDataStream& is,Chapter& chap)
{
    is >> chap.m_name;
    is >> chap.m_ressoucelist;
    is >> chap.m_chapterlist;
    return is;

}
