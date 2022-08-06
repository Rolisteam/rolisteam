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
class CSItem;
/**
 * @brief The Section class store data field for charactersheet.
 */
class CHARACTERSHEET_EXPORT Section : public CharacterSheetItem
{
    Q_OBJECT
public:
    Section();
    virtual ~Section();
    virtual bool hasChildren() override;
    virtual int getChildrenCount() const override;
    virtual CharacterSheetItem* getChildAt(int) const override;
    virtual CharacterSheetItem* getChildFromId(const QString&) const override;
    virtual QVariant getValueFrom(CharacterSheetItem::ColumnId, int role) const override;
    virtual void setValueFrom(CharacterSheetItem::ColumnId, QVariant) override;
    virtual bool mayHaveChildren() const override;
    virtual void appendChild(CharacterSheetItem*) override;
    void insertChild(CharacterSheetItem* item, int pos);
    virtual int indexOfChild(CharacterSheetItem* itm) override;
    virtual void save(QJsonObject& json, bool exp= false) override;
    virtual void load(const QJsonObject& json) override;
    virtual void setNewEnd(QPointF) override {}
    void copySection(Section* itm);
    bool removeChild(CharacterSheetItem*) override;
    bool deleteChild(CharacterSheetItem*) override;
    void setValueForAll(CharacterSheetItem* item, int col);
    virtual void saveDataItem(QJsonObject& json) override;
    virtual void loadDataItem(const QJsonObject& json) override;
    virtual void removeAll();
    void resetAllId(int& i);
    QList<CSItem*> allChildren();
    void setFieldInDictionnary(QHash<QString, QString>& dict) const override;

    void setOrig(CharacterSheetItem* orig) override;
    void changeKeyChild(QString oldkey, QString newKey, CharacterSheetItem* child) override;
    void getFieldFromPage(int pagePos, QList<CharacterSheetItem*>& list);
public slots:
    void buildDataInto(CharacterSheet* characterSheet);
signals:
    void addLineToTableField(CharacterSheetItem*);

private:
    QHash<QString, CharacterSheetItem*> m_dataHash;
    QStringList m_keyList;
};
#endif // SECTION_H
