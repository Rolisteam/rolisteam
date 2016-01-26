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
#include "resourcesnode.h"
/**
    * @brief chapter stores ressources and can be identified by a name, Resources can be subchapter or CleverURI
    * @brief it is part of the composite pattern
    */
class Chapter : public QObject,public ResourcesNode
{
    Q_OBJECT
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
    virtual ~Chapter();
    /**
    * @brief overridden method, is leaf or not
    */
    virtual bool hasChildren() const;
    bool mayHaveChildren() const;
    int getChildrenCount()const;

    virtual ResourcesNode* getChildAt(int) const;
    virtual int indexOf(ResourcesNode*);

    /**
    * @brief add resource into the chapter
    * @param CleverURI address
    */
    void addResource(ResourcesNode* );
    /**
    * @brief removes node (chapter or ressource)
    */
    bool removeChild(ResourcesNode* item);

    virtual bool contains(ResourcesNode *);

    void write(QDataStream& out) const;
    void read(QDataStream &in);

    QVariant getData(int);

    void insertChildAt(int row, ResourcesNode *uri);
signals:
    void openFile(CleverURI*, bool);
private:
    QList<ResourcesNode*> m_children;/// list of resource

    
    
    friend QDataStream& operator<<(QDataStream& os,const Chapter&);
    friend QDataStream& operator>>(QDataStream& is,Chapter&);
};
typedef QList<Chapter> ChapterList;
Q_DECLARE_METATYPE(Chapter)
Q_DECLARE_METATYPE(ChapterList)
#endif // CHAPTER_H
