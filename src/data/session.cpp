#include "session.h"

Session::Session()
{
    //m_chapterList = new QList<Chapter*>;
}
Session::Session(const Session& m)
{

}

Session::~Session()
{

}
Chapter* Session::addChapter(QString& name)
{
    Chapter* imp = new Chapter();
    imp->setName(name);
    m_chapterList.append(imp);
    return imp;
}

CleverURI* Session::addRessource(QString& uri, CleverURI::ContentType& type, Chapter* chapter)
{
    CleverURI* tp=new CleverURI(uri,type);
    if(chapter==NULL)
    {
        m_ressoucelist.append(tp);
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
 QList<Chapter*>& Session::chapterList()
 {
    return m_chapterList;
 }
 QList<CleverURI*>& Session::getUnclassedList()
 {
    return m_ressourcelist;
 }
 QDataStream& operator<<(QDataStream& out, const Session& con)
 {
   out << con.m_gameCount;
   out << con.m_time;
   return out;
 }

 QDataStream& operator>>(QDataStream& is,Session& con)
 {
   is >>(con.m_gameCount);
   is >>(con.m_time);
   return is;
 }

