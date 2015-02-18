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
#include "chapter.h"
#include <QDebug>
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
    return (m_ressoucelist.size()+m_chapterlist.size()>0)?true:false;
}
const QString& Chapter::getShortName() const
{
    return m_name;
}
void Chapter::addResource(CleverURI* cluri)
{
    m_ressoucelist.append(cluri);
}
Chapter* Chapter::addChapter(QString& name)
{
    Chapter* t = new Chapter();
    t->setShortName(name);
    m_chapterlist.append(t);
    return t;
}

void Chapter::setShortName(QString& name)
{
    m_name = name;
}

QList<CleverURI*>& Chapter::getResourceList()
{
    return m_ressoucelist;
}

QList<Chapter*>& Chapter::getChapterList()
{
    return m_chapterlist;
}
bool Chapter::removeRessourcesNode(RessourcesNode* item)
{
    CleverURI* itemURI= dynamic_cast<CleverURI*>(item);
    bool suppr=false;
    if(itemURI)
    {
          suppr= m_ressoucelist.removeOne(itemURI);
    }
    else
    {

        Chapter* imp= dynamic_cast<Chapter*>(item);
        suppr=m_chapterlist.removeOne(imp);

    }
    for(int i = 0;(i>m_chapterlist.size()) && (!suppr); i++)
    {
        suppr=m_chapterlist.at(i)->removeRessourcesNode(item);
    }

    return suppr;

//    QList<Chapter> m_chapterList;
//    QList<CleverURI> m_ressoucelist;
}
QDataStream& operator<<(QDataStream& os,const Chapter& chap)
{
    os << chap.m_name;

    os << chap.m_chapterlist.size();
    foreach(Chapter* chapter, chap.m_chapterlist)
      os << *chapter;

    os << chap.m_ressoucelist.size();
    foreach(CleverURI* uri, chap.m_ressoucelist)
      os << *uri;

//    os << chap.m_ressoucelist;
//    os << chap.m_chapterlist;
    return os;
}

QDataStream& operator>>(QDataStream& is,Chapter& chap)
{
    is >> chap.m_name;
    int sizeChapter=0;
    int sizeURI=0;


    is >> sizeChapter; //(con.m_chapterList);
    for(int i=0 ; i<sizeChapter ; i++)
    {
        Chapter* chapter = new Chapter;
        is >>*chapter;
        chap.m_chapterlist.append(chapter);
    }

    is >>sizeURI;
    for(int j=0 ; j<sizeURI ; j++)
    {
        CleverURI* uri = new CleverURI;
        is >>*uri;
        chap.m_ressoucelist.append(uri);
    }

    //is >> chap.m_ressoucelist;
    //is >> chap.m_chapterlist;
    return is;

}
