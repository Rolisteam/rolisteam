#include "session.h"
#include <QDebug>

Session::Session()
{

    //m_chapterList = new QList<Chapter*>;
}
Session::Session(const Session& m)
{
    m_chapterList = m.m_chapterList;
    m_ressoucelist = m.m_ressoucelist;
    m_time = m.m_time;
    m_gameCount = m.m_gameCount;
}

Session::~Session()
{

}
Chapter* Session::addChapter(QString& name)
{
    Chapter* imp = new Chapter();
    imp->setName(name);
    m_chapterList.append(*imp);
    return imp;
}
int Session::childrenCount()
{
        return (m_chapterList.size()+m_ressoucelist.size());
}
int Session::chapterCount()
{
    return m_chapterList.size();
}

CleverURI* Session::addRessource(QString& uri, CleverURI::ContentType& type, Chapter* chapter)
{
    CleverURI* tp=new CleverURI(uri,type);
    if(chapter==NULL)
    {
        m_ressoucelist.append(*tp);
    }
    else
    {
        chapter->addResource(tp);
    }
    return tp;
}

Chapter* Session::getChapter(int index)
{
    return NULL;
}
 QList<Chapter>& Session::chapterList()
 {
    return m_chapterList;
 }
 QList<CleverURI>& Session::getUnclassedList()
 {
    return m_ressoucelist;
 }
 QDataStream& operator<<(QDataStream& out, const Session& con)
 {
   out << con.m_gameCount;
   out << con.m_time;
   qDebug() << "SESSION " << con.m_chapterList.size();
   out << con.m_chapterList;
   out << con.m_ressoucelist;
      qDebug() << "SESSION  ressource" <<con.m_ressoucelist.size();
      return out;
 }

 QDataStream& operator>>(QDataStream& is,Session& con)
 {
   is >>(con.m_gameCount);
   is >>(con.m_time);
   is >> (con.m_chapterList);
   qDebug() << "SESSION " <<con.m_chapterList.size();
   is >> (con.m_ressoucelist);
   qDebug() << "SESSION  ressource" <<con.m_ressoucelist.size();
   return is;
 }

