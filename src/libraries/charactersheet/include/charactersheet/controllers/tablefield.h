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
#ifndef TABLEFIELD_H
#define TABLEFIELD_H

#include "charactersheet/charactersheetitem.h"
#include "charactersheet/controllers/tablemodel.h"
#include "fieldcontroller.h"
#include <QGraphicsItem>
#include <QLabel>
#include <QObject>
#include <QStandardItemModel>
#include <QWidget>
#include <charactersheet/charactersheet_global.h>

class TableFieldController;
/**
 * @brief The Field class managed text field in qml and datamodel.
 */
class CHARACTERSHEET_EXPORT TableFieldController : public FieldController
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* model READ model CONSTANT)
    Q_PROPERTY(int columnCount READ columnCount NOTIFY columnCountChanged FINAL)
    Q_PROPERTY(int displayedRow READ displayedRow NOTIFY displayedRowChanged FINAL)
public:
    enum ControlPosition
    {
        CtrlLeftTop,
        CtrlLeftBottom,
        CtrlTopLeft,
        CtrlTopRight,
        CtrlBottomLeft,
        CtrlBottomRight,
        CtrlRightTop,
        CtrlRightBottom
    };
    explicit TableFieldController(TreeSheetItem::TreeItemType type, bool addCount= true, QObject* parent= nullptr);
    explicit TableFieldController(bool addCount= true, QObject* parent= nullptr);
    explicit TableFieldController(QPointF topleft, bool addCount= true, QObject* parent= nullptr);
    virtual ~TableFieldController();

    TableModel* model() const;

    void init();

    // virtual void setCanvasField(CanvasField* canvasField) override;

    virtual QVariant valueFrom(TreeSheetItem::ColumnId, int role) const override;

    /// Overriden from charactersheetitem
    virtual bool hasChildren() override;
    virtual int childrenCount() const override;
    virtual TreeSheetItem* childFromId(const QString& id) const override;
    virtual TreeSheetItem* childAt(int) const override;
    virtual void save(QJsonObject& json, bool exp= false) override;
    virtual void load(const QJsonObject& json) override;
    virtual void copyField(TreeSheetItem* oldItem, bool copyData, bool sameId= true) override;

    ControlPosition position() const;
    void setPosition(const ControlPosition& position);
    void saveDataItem(QJsonObject& json) override;
    void loadDataItem(const QJsonObject& json) override;
    void setChildFieldData(const QJsonObject& json);

    void appendChild(TreeSheetItem*) override;

    Q_INVOKABLE QList<int> sumColumn() const;
    void setFieldInDictionnary(QHash<QString, QString>& dict) const override;

    int rowCount() const;
    int displayedRow() const;
    int columnCount() const;

public slots:
    void addLine();
    void addColumn();
    void removeColumn(int index);
    void removeLine(int line);
    void removeLastLine();

signals:
    void lineMustBeAdded(TableFieldController* table);
    void rowCountChanged();
    void columnCountChanged(bool, int);
    void displayedRowChanged();
    void requestUpdate();
    void cellValueChanged(const QString& tableId, int r, int c, const QString& id);

protected:
    void updateColumnSize();
    void setDisplayedRow(int rCount);

protected:
    ControlPosition m_position;
    std::unique_ptr<TableModel> m_model;

private:
    int m_displayedRow{1};
};

#endif // TABLEFIELD_H
