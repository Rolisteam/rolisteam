/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <QDebug>


#include "session.h"

Session::Session()
{



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
    imp->setShortName(name);
    m_chapterList.append(imp);
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
        m_ressoucelist.append(tp);
    }
    else
    {
        chapter->addResource(tp);
    }
    return tp;
}
void Session::removeRessourcesNode(RessourcesNode* item)
{
    CleverURI* itemURI= dynamic_cast<CleverURI*>(item);
    bool suppr=false;
    if(itemURI!=NULL)
    {
          suppr= m_ressoucelist.removeOne(itemURI);
    }
    else
    {
        Chapter* imp= dynamic_cast<Chapter*>(item);
        suppr=m_chapterList.removeOne(imp);

    }
    for(int i = 0;(i>m_chapterList.size()) && (!suppr); i++)
    {
        suppr=m_chapterList.at(i)->removeRessourcesNode(item);
    }



//    QList<Chapter> m_chapterList;
//    QList<CleverURI> m_ressoucelist;
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
    return m_ressoucelist;
}
QDataStream& operator<<(QDataStream& out, const Session& con)
{
    out << con.m_gameCount;
    out << con.m_time;
    //   qDebug() << "SESSION " << con.m_chapterList.size();
    out << con.m_chapterList.size();
    foreach(Chapter* chapter, con.m_chapterList)
      out << *chapter;

    out << con.m_ressoucelist.size();
    foreach(CleverURI* uri, con.m_ressoucelist)
      out << *uri;

    //   qDebug() << "SESSION  ressource" <<con.m_ressoucelist.size();
    return out;
}

QDataStream& operator>>(QDataStream& is,Session& con)
{
    is >>(con.m_gameCount);
    is >>(con.m_time);
    int sizeChapter=0;
    int sizeURI=0;

    is >> sizeChapter; //(con.m_chapterList);
    for(int i=0 ; i<sizeChapter ; i++)
    {
        Chapter* chapter = new Chapter;
        is >>*chapter;
        con.m_chapterList.append(chapter);
    }

    is >>sizeURI;
    for(int j=0 ; j<sizeURI ; j++)
    {
        CleverURI* uri = new CleverURI;
        is >>*uri;
        con.m_ressoucelist.append(uri);
    }
    //is >> (con.m_ressoucelist);

    return is;
}
