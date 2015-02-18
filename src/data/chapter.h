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
#ifndef CHAPTER_H
#define CHAPTER_H

#include <QList>
#include "cleveruri.h"
#include "ressourcesnode.h"

class Chapter : public RessourcesNode
{
public:
    Chapter();
    Chapter(const Chapter& m);
    ~Chapter();
    virtual const QString& getShortName() const;
    virtual void setShortName(QString& name);
    virtual bool hasChildren() const;
    void addResource(CleverURI* );
    Chapter* addChapter(QString& name);
    bool removeRessourcesNode(RessourcesNode* item);

    QList<CleverURI*>& getResourceList();
    QList<Chapter*>& getChapterList();
private:
    QList<CleverURI*> m_ressoucelist;
    QList<Chapter*> m_chapterlist;
    QString m_name;


    friend QDataStream& operator<<(QDataStream& os,const Chapter&);
    friend QDataStream& operator>>(QDataStream& is,Chapter&);
};
typedef QList<Chapter> ChapterList;
Q_DECLARE_METATYPE(Chapter)
Q_DECLARE_METATYPE(ChapterList)
#endif // CHAPTER_H
