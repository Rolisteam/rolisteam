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
#ifndef FIELDMODEL_H
#define FIELDMODEL_H

#include <QAbstractItemModel>
#include <QJsonObject>
#include <QObject>
#include <QTextStream>

#include "charactersheet/charactersheetitem.h"
#include "charactersheet/field.h"
#include "charactersheet/section.h"

class EditorController;

namespace Formula
{
class FormulaManager;
}
/**
s * @brief The Column class
 */
class Column
{

public:
    /**
     * @brief Column
     */
    Column(QString, CharacterSheetItem::ColumnId);
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
     * @brief getPos
     * @return
     */
    CharacterSheetItem::ColumnId getPos() const;
    /**
     * @brief setPos
     * @param pos
     */
    void setPos(const CharacterSheetItem::ColumnId& pos);

private:
    QString m_name;
    CharacterSheetItem::ColumnId m_pos;
};

/**
 * @brief The FieldModel class
 */
class FieldModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit FieldModel(QObject* parent= nullptr);
    virtual ~FieldModel();

    // Model API
    QVariant data(const QModelIndex& index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void appendField(CSItem* f);
    void save(QJsonObject& json, bool exp= false);
    void load(const QJsonObject& json);
    void generateQML(QTextStream& out, int indentation, bool isTable= false);
    Q_INVOKABLE QString getValue(const QString& key);
    QList<CharacterSheetItem*> children();
    QList<CSItem*> allChildren();
    void removePageId(int);
    Section* getRootSection() const;
    void setRootSection(Section* rootSection);
    void removeItem(QModelIndex& index);
    void removeField(FieldController* field);
    void setValueForAll(QModelIndex& index);
    void insertField(CSItem* field, CharacterSheetItem* parent, int pos);
    void getFieldFromPage(int pagePos, QList<CharacterSheetItem*>& list);
    FieldController* getFieldFromIndex(const QModelIndex& index);
signals:
    void valuesChanged(QString valueKey, QString value);
    void modelChanged();
    void fieldAdded(CSItem* item);

public slots:
    void updateItem(CSItem*);
    void clearModel();
    void resetAllId();

private:
    QHash<QString, QString> buildDicto();

private:
    std::unique_ptr<Section> m_rootSection;
    std::unique_ptr<Formula::FormulaManager> m_formulaManager;
    QList<Column*> m_colunm;
    QStringList m_alignList;
};

#endif // FIELDMODEL_H
