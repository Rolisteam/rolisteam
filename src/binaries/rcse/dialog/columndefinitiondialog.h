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
#ifndef COLUMNDEFINITIONDIALOG_H
#define COLUMNDEFINITIONDIALOG_H

#include <QDialog>

#include "fieldmodel.h"
#include <QList>

class QUndoStack;
namespace Ui
{
class ColumnDefinitionDialog;
}

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

class ColumnDefinitionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColumnDefinitionDialog(QWidget* parent= nullptr);
    ~ColumnDefinitionDialog();

    FieldModel* model() const;
    void setModel(FieldModel* model);

    void load(QJsonObject& json);
    void save(QJsonObject& json);

signals:
    void lineCountChanged(int i);
    void positionChanged(int i);
    void columnCountChanged(int i);

public slots:
    void setData(QList<HandleItem*>, qreal widthTotal, int line, qreal height);

private:
    Ui::ColumnDefinitionDialog* ui;
    FieldModel* m_model;
};

#endif // COLUMNDEFINITIONDIALOG_H
