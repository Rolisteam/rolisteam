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
 * @brief The Section class store data field for charactersheet.
 */
class Section : public CharacterSheetItem
{
    Q_OBJECT
public:
    /**
     * @brief Section constructor
     */
    Section();
    ~Section();

    /**
     * @brief hasChildren
     * @return
     */
    virtual bool hasChildren();
    /**
     * @brief getChildrenCount
     * @return
     */
    virtual int getChildrenCount() const;
    /**
     * @brief getChildAt
     * @return
     */
    virtual CharacterSheetItem* getChildAt(int) const;
    /**
     * @brief getChildAt
     * @return
     */
    virtual CharacterSheetItem* getChildAt(QString) const;

    /**
     * @brief getValueFrom
     * @return
     */
    virtual QVariant getValueFrom(CharacterSheetItem::ColumnId,int role) const;
    /**
     * @brief setValueFrom
     */
    virtual void setValueFrom(CharacterSheetItem::ColumnId,QVariant);
    /**
     * @brief mayHaveChildren
     * @return
     */
    virtual bool mayHaveChildren() const;
    /**
     * @brief appendChild
     */
    virtual void appendChild(CharacterSheetItem*);
    void insertChild(CharacterSheetItem* item,int pos);

    /**
     * @brief indexOfChild
     * @param itm
     * @return
     */
    virtual int indexOfChild(CharacterSheetItem *itm);
    /**
     * @brief getName
     * @return
     */
    QString getName() const;
    /**
     * @brief setName
     * @param name
     */
    void setName(const QString &name);
    /**
     * @brief save
     * @param json
     * @param exp
     */
    virtual void save(QJsonObject& json,bool exp=false);
    /**
     * @brief load
     * @param json
     * @param scene
     */
    virtual void load(QJsonObject& json,QList<QGraphicsScene*> scene);
    /**
     * @brief setNewEnd
     */
    virtual void setNewEnd(QPointF){}
    /**
     * @brief getItemType
     * @return
     */
    virtual CharacterSheetItem::CharacterSheetItemType getItemType() const;
    /**
     * @brief copySection
     * @param itm
     */
    void copySection(Section* itm);
    /**
     * @brief removeChild
     * @return
     */
    bool removeChild(CharacterSheetItem*);
    bool deleteChild(CharacterSheetItem*);
    /**
     * @brief setValueForAll
     * @param item
     * @param col
     */
    void setValueForAll(CharacterSheetItem* item,int col);

    /**
     * @brief saveDataItem saves only data, no info about how to display the item.
     * @param json
     */
    virtual void saveDataItem(QJsonObject& json);
    /**
     * @brief loadDataItem load core data: id, value, label.
     * @param json
     */
    virtual void loadDataItem(QJsonObject& json);
    /**
     * @brief removeAll
     */
    virtual void removeAll();
    /**
     * @brief resetAllId
     * @param i
     */
    void resetAllId(int &i);

    void setOrig(CharacterSheetItem *orig);
    void changeKeyChild(QString oldkey, QString newKey, CharacterSheetItem *child);
    void getFieldFromPage(int pagePos, QList<CharacterSheetItem *> &list);
public slots:
    /**
     * @brief fillList
     * @param result
     * @param character
     */
    void buildDataInto(CharacterSheet* characterSheet);
signals:
    void addLineToTableField(CharacterSheetItem*);
private:
    QHash<QString,CharacterSheetItem*> m_dataHash;
    QStringList m_keyList;
    QString m_name;
};
#endif // SECTION_H
