/***************************************************************************
 * Copyright (C) 2016 by Renaud Guezennec                                   *
 * https://rolisteam.org/                                                *
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
#include "charactersheet/charactersheet.h"
#include "charactersheet/charactersheetitem.h"
#include <QPointF>

#include <charactersheet/charactersheet_global.h>

class CharacterSheet;
class EditorController;
/**
 * @brief The Section class store data field for charactersheet.
 */
class CHARACTERSHEET_EXPORT Section : public CharacterSheetItem
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
    virtual bool hasChildren() override;
    /**
     * @brief getChildrenCount
     * @return
     */
    virtual int getChildrenCount() const override;
    /**
     * @brief getChildAt
     * @return
     */
    virtual CharacterSheetItem* getChildAt(int) const override;
    /**
     * @brief getChildAt
     * @return
     */
    virtual CharacterSheetItem* getChildFromId(const QString&) const override;

    /**
     * @brief getValueFrom
     * @return
     */
    virtual QVariant getValueFrom(CharacterSheetItem::ColumnId, int role) const override;
    /**
     * @brief setValueFrom
     */
    virtual void setValueFrom(CharacterSheetItem::ColumnId, QVariant) override;
    /**
     * @brief mayHaveChildren
     * @return
     */
    virtual bool mayHaveChildren() const override;
    /**
     * @brief appendChild
     */
    virtual void appendChild(CharacterSheetItem*) override;
    void insertChild(CharacterSheetItem* item, int pos);

    /**
     * @brief indexOfChild
     * @param itm
     * @return
     */
    virtual int indexOfChild(CharacterSheetItem* itm) override;
    /**
     * @brief getName
     * @return
     */
    QString getName() const;
    /**
     * @brief setName
     * @param name
     */
    void setName(const QString& name);
    /**
     * @brief save
     * @param json
     * @param exp
     */
    virtual void save(QJsonObject& json, bool exp= false) override;
    /**
     * @brief load
     * @param json
     * @param scene
     */
    virtual void load(const QJsonObject& json, EditorController* ctrl) override;
    /**
     * @brief setNewEnd
     */
    virtual void setNewEnd(QPointF) override {}
    /**
     * @brief getItemType
     * @return
     */
    virtual CharacterSheetItem::CharacterSheetItemType getItemType() const override;
    /**
     * @brief copySection
     * @param itm
     */
    void copySection(Section* itm);
    /**
     * @brief removeChild
     * @return
     */
    bool removeChild(CharacterSheetItem*) override;
    bool deleteChild(CharacterSheetItem*) override;
    /**
     * @brief setValueForAll
     * @param item
     * @param col
     */
    void setValueForAll(CharacterSheetItem* item, int col);

    /**
     * @brief saveDataItem saves only data, no info about how to display the item.
     * @param json
     */
    virtual void saveDataItem(QJsonObject& json) override;
    /**
     * @brief loadDataItem load core data: id, value, label.
     * @param json
     */
    virtual void loadDataItem(const QJsonObject& json) override;
    /**
     * @brief removeAll
     */
    virtual void removeAll();
    /**
     * @brief resetAllId
     * @param i
     */
    void resetAllId(int& i);

    void setOrig(CharacterSheetItem* orig) override;
    void changeKeyChild(QString oldkey, QString newKey, CharacterSheetItem* child) override;
    void getFieldFromPage(int pagePos, QList<CharacterSheetItem*>& list);
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
    QHash<QString, CharacterSheetItem*> m_dataHash;
    QStringList m_keyList;
    QString m_name;
};
#endif // SECTION_H
