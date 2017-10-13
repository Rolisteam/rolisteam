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

#include <QJsonObject>
#include <QObject>
#include <QAbstractItemModel>
#include <QTextStream>

#include "field.h"
//#include "charactersheetbutton.h"
#include "charactersheetitem.h"

#include "section.h"
class Canvas;

/**
s * @brief The Column class
 */
class Column
{

public:
    /**
     * @brief Column
     */
    Column(QString,CharacterSheetItem::ColumnId);
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
     * @brief getPos
     * @return
     */
    CharacterSheetItem::ColumnId getPos() const;
    /**
     * @brief setPos
     * @param pos
     */
    void setPos(const CharacterSheetItem::ColumnId &pos);

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
    explicit FieldModel(QObject *parent = 0);
    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex &index, int role) const;
    /**
     * @brief index
     * @param row
     * @param column
     * @param parent
     * @return
     */
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    /**
     * @brief parent
     * @param child
     * @return
     */
    QModelIndex parent(const QModelIndex &child) const;
    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(const QModelIndex &parent) const;
    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    int columnCount(const QModelIndex &parent) const;
    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    /**
     * @brief setData
     * @param index
     * @param value
     * @param role
     * @return
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    /**
     * @brief appendField
     * @param f
     */
    void appendField(CSItem* f);
    /**
     * @brief flags
     * @param index
     * @return
     */
    Qt::ItemFlags flags(const QModelIndex &index) const;
    /**
     * @brief save
     * @param json
     * @param exp
     */
    void save(QJsonObject& json,bool exp=false);
    /**
     * @brief load
     * @param json
     * @param scene
     */
    void load(QJsonObject& json,QList<Canvas*> scene);
    /**
     * @brief generateQML
     * @param out
     * @param sec
     */
    void generateQML(QTextStream& out,CharacterSheetItem::QMLSection sec, bool isTable = false);
    /**
    *
    */
    Q_INVOKABLE QString getValue(const QString& key);
    /**
     * @brief children
     * @return
     */
    QList<CharacterSheetItem*> children();
    /**
     * @brief removePageId
     */
    void removePageId(int);
    /**
     * @brief getRootSection
     * @return
     */
    Section* getRootSection() const;
    /**
     * @brief setRootSection
     * @param rootSection
     */
    void setRootSection(Section *rootSection);
    /**
     * @brief removeItem
     * @param index
     */
    void removeItem(QModelIndex& index);
    void removeField(Field* field);
    /**
     * @brief setValueForAll
     * @param index
     */
    void setValueForAll(QModelIndex& index);
    /**
     * @brief insertField
     * @param field
     * @param parent
     * @param pos
     */
    void insertField(CSItem *field, CharacterSheetItem *parent, int pos);
    /**
     * @brief getFieldFromPage
     * @param pagePos
     * @param list
     */
    void getFieldFromPage(int pagePos, QList<CharacterSheetItem *> &list);
    /**
     * @brief getFieldFromIndex
     * @param index
     */
    Field* getFieldFromIndex(const QModelIndex& index);
signals:
    /**
     * @brief valuesChanged
     * @param valueKey
     * @param value
     */
    void valuesChanged(QString valueKey,QString value);
    /**
     * @brief modelChanged
     */
    void modelChanged();

public slots:
    /**
     * @brief updateItem
     */
    void updateItem(CSItem* );
    void clearModel();
    void resetAllId();
private:
    QList<Column*> m_colunm;
    Section* m_rootSection;
    QStringList m_alignList;

};

#endif // FIELDMODEL_H
