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
#include <utils/directaccessdatastructure.h>

class CharacterSheet;
class EditorController;
class CSItem;

/**
 * @brief The Section class store data field for charactersheet.
 */
class CHARACTERSHEET_EXPORT Section : public TreeSheetItem
{
    Q_OBJECT
public:
    Section();
    virtual ~Section();
    bool hasChildren() override;
    int childrenCount() const override;
    TreeSheetItem* childAt(int) const override;
    TreeSheetItem* childFromId(const QString&) const override;

    bool mayHaveChildren() const override;
    void appendChild(TreeSheetItem*) override;
    void insertChild(TreeSheetItem* item, int pos);
    int indexOfChild(TreeSheetItem* itm) override;

    void copySection(Section* itm);
    bool removeChild(TreeSheetItem*) override;
    bool deleteChild(TreeSheetItem*) override;
    void setValueForAll(TreeSheetItem* item, int col);
    void setOrig(TreeSheetItem* orig) override;

    virtual void save(QJsonObject& json, bool exp= false) override;
    virtual void load(const QJsonObject& json) override;
    /*    virtual void saveDataItem(QJsonObject& json) override;
        virtual void loadDataItem(const QJsonObject& json) override;*/
    virtual void removeAll();
    void resetAllId(int& i);
    QList<CSItem*> allChildren() const;
    void setFieldInDictionnary(QHash<QString, QString>& dict) const override;

    void changeKeyChild(const QString& oldkey, const QString& newKey, TreeSheetItem* child) override;
    QList<CSItem*> fieldFromPage(int pagePos);

public slots:
    void buildDataInto(CharacterSheet* characterSheet);
signals:
    void addLineToTableField(TreeSheetItem*);

private:
    DirectAccessDataStructure<QString, TreeSheetItem*> m_data;
};

#endif // SECTION_H
