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
    return new CleverURI();
}

Chapter* Session::getChapter(int index)
{
    return NULL;
}
 QList<Chapter*>& Session::chapterList()
 {
    return m_chapterList;
 }
