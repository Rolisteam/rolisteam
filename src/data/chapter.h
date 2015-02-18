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
/**
    * @brief chapter stores ressources and can be identified by a name, Resources can be subchapter or CleverURI
    * @brief it is part of the composite pattern
    */
class Chapter : public RessourcesNode
{
public:
    /**
    * @brief default constructor
    */
    Chapter();
    /**
    * @brief copy constructor
    */
    Chapter(const Chapter& m);
    /**
    * @brief destructor
    */
    ~Chapter();
    /**
    * @brief overridden method to get the shortname. Shortname is just the filename (without path)
    * @return the shortname
    */
    virtual const QString& getShortName() const;
    /**
    * @brief define the shortname (overridden method)
    * @param new value
    */
    virtual void setShortName(QString& name);
    /**
    * @brief overridden method, is leaf or not
    */
    virtual bool hasChildren() const;
    /**
    * @brief add resource into the chapter
    * @param CleverURI address
    */
    void addResource(CleverURI* );
    /**
    * @brief create and add chapter as a child this one,
    * @param name of the chapter
    * @return chapter's created address
    */
    Chapter* addChapter(QString& name);
    /**
    * @brief removes node (chapter or ressource)
    */
    bool removeRessourcesNode(RessourcesNode* item);
    /**
    * @brief accessor to the resources list
    */
    QList<CleverURI*>& getResourceList();
    /**
    * @brief accessor to the chapter list
    */
    QList<Chapter*>& getChapterList();
private:
    
    QList<CleverURI*> m_ressoucelist;/// list of resource
    QList<Chapter*> m_chapterlist;/// list of chapter
    QString m_name; /// name of chapter
    
    
    friend QDataStream& operator<<(QDataStream& os,const Chapter&);
    friend QDataStream& operator>>(QDataStream& is,Chapter&);
};
typedef QList<Chapter> ChapterList;
Q_DECLARE_METATYPE(Chapter)
Q_DECLARE_METATYPE(ChapterList)
#endif // CHAPTER_H
