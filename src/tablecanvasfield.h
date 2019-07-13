/***************************************************************************
 * Copyright (C) 2018 by Renaud Guezennec                                   *
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
#ifndef TABLECANVASFIELD_H
#define TABLECANVASFIELD_H

#include <QObject>
#include <memory>

#include "canvasfield.h"
#include "charactersheetitem.h"
#include "dialog/columndefinitiondialog.h"

class TableCanvasField;
class LineModel;
class TableField;
class HandleItem : public QGraphicsObject
{
public:
    enum MOTION
    {
        X_AXIS,
        Y_AXIS
    };
    /**
     * @brief HandleItem
     * @param point
     * @param parent
     */
    explicit HandleItem(QGraphicsObject* parent= nullptr);
    /**
     * @brief ~HandleItem
     */
    virtual ~HandleItem();
    /**
     * @brief itemChange
     * @param change
     * @param value
     * @return
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant& value);
    /**
     * @brief boundingRect
     * @return
     */
    QRectF boundingRect() const;
    /**
     * @brief paint
     * @param painter
     * @param option
     * @param widget
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);
    void load(QJsonObject& json);
    void save(QJsonObject& json);

protected:
    /**
     * @brief ChildPointItem::mouseMoveEvent
     * @param event
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    /**
     * @brief ChildPointItem::mouseReleaseEvent
     * @param event
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:
    QPointF m_startPoint;
    bool m_posHasChanged;
    MOTION m_currentMotion;
};

class ButtonCanvas : public QGraphicsObject
{
    Q_OBJECT
public:
    ButtonCanvas();
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);
    QString msg() const;
    void setMsg(const QString& msg);
    QRectF rect() const;
    void setRect(const QRectF& rect);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

signals:
    void clicked();

private:
    QString m_msg;
    QRectF m_rect;
};

class TableCanvasField : public CanvasField
{
    Q_OBJECT
public:
    explicit TableCanvasField(Field* field);
    virtual ~TableCanvasField();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* a= nullptr);

    bool hasFocusOrChild();

    int colunmCount() const;
    void setColunmCount(int colunmCount);

    int lineCount() const;
    void setLineCount(int lineCount);

    int getColumnWidth(int c);
    int getLineHeight();

    void fillLineModel(LineModel* model, TableField* parent);

    Field* generateSubField(int i);

    void generateSubFields(QTextStream& out);

    void load(QJsonObject& json, EditorController* ctrl);
    void save(QJsonObject& json);

    CharacterSheetItem* getRoot();

public slots:
    void addColumn();
    void removeColumn();
    void addLine();
    void defineColumns();

protected:
    virtual void setMenu(QMenu& menu);

private:
    int m_colunmCount;
    int m_lineCount;

    QList<HandleItem*> m_handles;

    ButtonCanvas* m_addColumn= nullptr;
    ButtonCanvas* m_addLine= nullptr;
    QAction* m_defineColumns= nullptr;

    std::unique_ptr<ColumnDefinitionDialog> m_dialog;
    bool m_dataReset;
    bool m_columnDefined= false;
};

#endif // TABLECANVASFIELD_H
