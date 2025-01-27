/***************************************************************************
 * Copyright (C) 2014 by Renaud Guezennec                                   *
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
#ifndef CHARACTERSHEETITEM_H
#define CHARACTERSHEETITEM_H

#include <QJsonObject>
#include <QTextStream>
#include <QVariant>

#include <charactersheet/charactersheet_global.h>

class QGraphicsScene;
class EditorController;
/**
 * @brief The Item class
 */
class CHARACTERSHEET_EXPORT TreeSheetItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(TreeSheetItem::TreeItemType itemType READ itemType CONSTANT)
    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly NOTIFY readOnlyChanged FINAL)
public:
    enum TreeItemType
    {
        SectionItem,
        FieldItem,
        TableItem,
        SliderItem,
        CellValue
    };
    Q_ENUM(TreeItemType)

    enum ColumnId
    {
        ID,
        LABEL,
        VALUE,
        VALUES,
        TYPE,
        X,
        Y,
        WIDTH,
        HEIGHT,
        FitFont,
        FONT,
        TEXT_ALIGN,
        TEXTCOLOR,
        BGCOLOR,
        BORDER,
        PAGE,
        TOOLTIP
    };
    Q_ENUM(ColumnId)

    TreeSheetItem(TreeSheetItem::TreeItemType type, QObject* parent= nullptr);

    virtual bool mayHaveChildren() const;
    virtual bool hasChildren();
    virtual int childrenCount() const;
    virtual void appendChild(TreeSheetItem*);

    virtual TreeSheetItem* childFromId(const QString& id) const;
    virtual TreeSheetItem* childAt(int) const;

    virtual QString id() const;
    virtual QString path() const;

    void setId(const QString& id);
    virtual QVariant valueFrom(TreeSheetItem::ColumnId, int role) const= 0;
    virtual void setValueFrom(TreeSheetItem::ColumnId, const QVariant& data)= 0;
    virtual void setFormula(const QString& formula);

    TreeSheetItem* parentTreeItem() const;
    void setParent(TreeSheetItem* parent);
    int rowInParent();
    virtual int indexOfChild(TreeSheetItem* itm);

    TreeSheetItem::TreeItemType itemType() const;
    virtual bool removeChild(TreeSheetItem*);
    virtual bool deleteChild(TreeSheetItem*);
    virtual void setFieldInDictionnary(QHash<QString, QString>& dict) const= 0;
    virtual void changeKeyChild(const QString& oldkey, const QString& newKey, TreeSheetItem* child);

    virtual void setOrig(TreeSheetItem* m_origine)= 0;
    virtual void save(QJsonObject& json, bool exp= false) {}
    virtual void load(const QJsonObject& json) {}
    virtual void saveDataItem(QJsonObject& json) {}
    virtual void loadDataItem(const QJsonObject& json) {}

    bool readOnly() const;
    void setReadOnly(bool newReadOnly);

signals:
    void characterSheetItemChanged(TreeSheetItem* item);
    void idChanged(const QString& old, const QString& newId);

    void readOnlyChanged();

protected:
    TreeSheetItem::TreeItemType m_itemType;
    TreeSheetItem* m_parent;
    QString m_id;
    bool m_readOnly{false};
};

#endif // CHARACTERSHEETITEM_H
