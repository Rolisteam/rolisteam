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
#ifndef CHARACTERSHEETITEM_H
#define CHARACTERSHEETITEM_H

#include <QVariant>
#include <QJsonObject>
#include <QTextStream>

class QGraphicsScene;
/**
 * @brief The Item class
 */
class CharacterSheetItem
{
public:
    enum ColumnId {NAME,X,Y,WIDTH,HEIGHT,BORDER,TEXT_ALIGN,BGCOLOR,TEXTCOLOR,VALUES};
    enum QMLSection {FieldSec,ConnectionSec};
    Item();
    virtual bool hasChildren();
    virtual int getChildrenCount();

    virtual CharacterSheetItem* getChildAt(QString) const=0;
    virtual CharacterSheetItem* getChildAt(int) const;


    virtual QVariant getValue(CharacterSheetItem::ColumnId) const;
    virtual void setValue(CharacterSheetItem::ColumnId,QVariant data)=0;

    QString name() const;
    void setName(const QString &name);

        virtual QString getPath();

    virtual bool mayHaveChildren();
    virtual void appendChild(Item*);
    CharacterSheetItem *getParent() const;
    void setParent(CharacterSheetItem *parent);

    int rowInParent();

    virtual int indexOfChild(CharacterSheetItem *itm);
    virtual void save(QJsonObject& json,bool exp=false)=0;
    virtual void load(QJsonObject& json,QGraphicsScene* scene)=0;

    virtual void generateQML(QTextStream& out,Item::QMLSection sec)=0;
    virtual void setNewEnd(QPointF nend)=0;

    bool isReadOnly() const;
    void setReadOnly(bool readOnly);

private:
    CharacterSheetItem* m_parent;
    QString m_name;
    QStringList m_value;
    bool m_readOnly;
};

#endif // CHARACTERSHEETITEM_H
