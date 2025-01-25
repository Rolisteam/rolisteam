/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef LINKITEM_H
#define LINKITEM_H

#include "mindmap/mindmap_global.h"
#include <QList>
#include <QQuickItem>

#include "mindmap/data/linkcontroller.h"

typedef QList<QPointF> PointList;
namespace mindmap
{

class MINDMAP_EXPORT LinkItem : public QQuickItem
{
    Q_OBJECT
    QML_NAMED_ELEMENT(MindLink)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(PointList points READ points WRITE setPoints NOTIFY pointsChanged)
    Q_PROPERTY(qreal horizontalOffset READ horizontalOffset NOTIFY horizontalOffsetChanged)
    Q_PROPERTY(qreal verticalOffset READ verticalOffset NOTIFY verticalOffsetChanged)
    Q_PROPERTY(LinkController* controller READ controller WRITE setController NOTIFY controllerChanged)
    Q_PROPERTY(bool editing READ editing WRITE setEditing NOTIFY editingChanged FINAL)
    Q_PROPERTY(bool writable READ writable WRITE setWritable NOTIFY writableChanged FINAL REQUIRED)
public:
    LinkItem();

    PointList points() const;
    QColor color() const;
    qreal horizontalOffset() const;
    qreal verticalOffset() const;

    LinkController* controller() const;
    void setController(LinkController* newController);

    bool editing() const;
    void setEditing(bool newEditing);

    bool writable() const;
    void setWritable(bool newWritable);

public slots:
    void setPoints(const PointList& list);
    void setColor(QColor color);

signals:
    void horizontalOffsetChanged();
    void pointsChanged();
    void colorChanged();
    void selected(bool b);
    void verticalOffsetChanged();
    void controllerChanged();
    void editingChanged();
    void writableChanged();

protected:
    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* evenv) override;

protected slots:
    void setHorizontalOffset(qreal r);
    void setVerticalOffset(qreal r);

private:
    QPointer<LinkController> m_controller;
    QColor m_color= Qt::black;
    PointList m_points;
    bool m_colorChanged= false;
    qreal m_horizontalOffset;
    qreal m_verticalOffset;
    bool m_editing{false};
    bool m_writable{false};
};
} // namespace mindmap
#endif // LINKITEM_H
