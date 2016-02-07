/***************************************************************************
* Copyright (C) 2014 by Renaud Guezennec                                   *
* http://www.rolisteam.org/                                                *
*                                                                          *
*  This file is part of rcse                                               *
*                                                                          *
* rcse is free software; you can redistribute it and/or modify             *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* rcse is distributed in the hope that it will be useful,                  *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program; if not, write to the                            *
* Free Software Foundation, Inc.,                                          *
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
***************************************************************************/
#ifndef ITEM_H
#define ITEM_H

#include <QVariant>
#include <QJsonObject>
#include <QTextStream>

class QGraphicsScene;
/**
 * @brief The Item class
 */
class Item
{
public:
    enum ColumnId {NAME,X,Y,WIDTH,HEIGHT,BORDER,TEXT_ALIGN,BGCOLOR,TEXTCOLOR};
    enum QMLSection {FieldSec,ConnectionSec};
    Item();
    virtual bool hasChildren();
    virtual int getChildrenCount();
    virtual Item* getChildAt(int);
    virtual QVariant getValue(Item::ColumnId) const;
    virtual void setValue(Item::ColumnId,QVariant data)=0;
    virtual bool mayHaveChildren();
    virtual void appendChild(Item*);
    Item *getParent() const;
    void setParent(Item *parent);

    int row();

    virtual int indexOfChild(Item *itm);
    virtual void save(QJsonObject& json,bool exp=false)=0;
    virtual void load(QJsonObject& json,QGraphicsScene* scene)=0;

    virtual void generateQML(QTextStream& out,Item::QMLSection sec)=0;
private:
    Item* m_parent;
};

#endif // ITEM_H
