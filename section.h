/***************************************************************************
* Copyright (C) 2016 by Renaud Guezennec                                   *
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
#ifndef SECTION_H
#define SECTION_H
#include "charactersheetitem.h"
#include "charactersheet.h"
#include <QPointF>

class CharacterSheet;
/**
 * @brief The Section class
 */
class Section : public CharacterSheetItem
{
public:
    Section();

    virtual bool hasChildren();
    virtual int getChildrenCount() const;
    virtual CharacterSheetItem* getChildAt(int) const;
    virtual CharacterSheetItem* getChildAt(QString) const;


    virtual QVariant getValueFrom(CharacterSheetItem::ColumnId) const;
    virtual void setValueFrom(CharacterSheetItem::ColumnId,QVariant);
    virtual bool mayHaveChildren();
    virtual void appendChild(CharacterSheetItem*);
    virtual int indexOfChild(CharacterSheetItem *itm);
    QString getName() const;
    void setName(const QString &name);
    virtual void save(QJsonObject& json,bool exp=false);
    virtual void load(QJsonObject& json,QList<QGraphicsScene*> scene);
    virtual void generateQML(QTextStream &out,CharacterSheetItem::QMLSection);
    virtual void setNewEnd(QPointF){}
    virtual CharacterSheetItem::CharacterSheetItemType getItemType() const;
    void copySection(Section* itm);
    bool removeChild(CharacterSheetItem*);

public slots:
    void fillList(QList<CharacterSheetItem *> *result, CharacterSheet *character);
private:
    QHash<QString,CharacterSheetItem*> m_dataHash;
    QStringList m_keyList;
    QString m_name;
};
#endif // SECTION_H
