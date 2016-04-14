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
#include "charactersheetbutton.h"
#include "charactersheetitem.h"

#include "section.h"
//#include "canvas.h"
class Canvas;

/**
s * @brief The Column class
 */
class Column
{

public:
    Column(QString,CharacterSheetItem::ColumnId);
    QString getName() const;
    void setName(const QString &name);


    CharacterSheetItem::ColumnId getPos() const;
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

    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    void appendField(CSItem* f);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    void save(QJsonObject& json,bool exp=false);
    void load(QJsonObject& json,QList<Canvas*> scene);

    void generateQML(QTextStream& out,CharacterSheetItem::QMLSection sec);

    Q_INVOKABLE QString getValue(const QString& key);

    QList<CharacterSheetItem*> children();

    void removePageId(int);

    Section *getRootSection() const;
    void setRootSection(Section *rootSection);

    void removeItem(QModelIndex& index);

signals:
    void valuesChanged(QString valueKey,QString value);

public slots:
    void updateItem(CSItem* );
private:
    QList<Column*> m_colunm;
    Section* m_rootSection;

};

#endif // FIELDMODEL_H
