/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#ifndef POSITIONEDITEM_H
#define POSITIONEDITEM_H

#include <QPointer>
#include <QRectF>
#include <QSizeF>
#include <vector>

#include "minditem.h"
#include "mindmap/mindmap_global.h"

namespace mindmap
{
class LinkController;
class MINDMAP_EXPORT PositionedItem : public MindItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QPointF centerPoint READ centerPoint NOTIFY centerPointChanged)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(bool isDragged READ isDragged WRITE setDragged NOTIFY isDraggedChanged)
    Q_PROPERTY(bool open READ open WRITE setOpen NOTIFY openChanged)
    Q_PROPERTY(bool hasLink READ hasLink NOTIFY hasLinkChanged)
    Q_PROPERTY(bool locked READ isLocked WRITE setLocked NOTIFY lockedChanged)
    Q_PROPERTY(int mass READ mass WRITE setMass NOTIFY massChanged)

public:
    explicit PositionedItem(Type type, QObject* parent= nullptr);

    QPointF position() const;
    QPointF centerPoint() const;
    qreal width() const;
    qreal height() const;
    QRectF boundingRect() const;
    bool isDragged() const;
    int mass() const;
    PositionedItem* parentNode() const;
    bool hasLink() const;
    bool open() const;

    QString parentId() const;

    // Move
    void setNextPosition(const QPointF& pos, LinkController* emiter);
    QVector2D getVelocity() const;
    void setVelocity(const QVector2D& velocity);
    QVector2D getAcceleration() const;
    void setAcceleration(const QVector2D& acceleration);
    void applyForce(const QVector2D& force);
    const std::vector<QPointer<LinkController>>& subLinks() const;
    int subNodeCount() const;

    void addLink(LinkController* link);
    void removeLink(LinkController* link);

    bool isLocked() const;
    void setLocked(bool newLocked);

public slots:
    void setDragged(bool isdragged);
    void setPosition(const QPointF& p);
    void setWidth(qreal w);
    void setHeight(qreal h);
    void setMass(int m);
    void setParentNode(PositionedItem* parent);
    void setLinkVisibility();
    void translate(const QPointF& motion);
    void setOpen(bool b);

signals:
    void itemDragged(const QPointF motion);
    void positionChanged(const QPointF motion);
    void centerPointChanged();
    void widthChanged();
    void heightChanged();
    void isDraggedChanged();
    void massChanged();
    void hasLinkChanged();
    void openChanged();
    void lockedChanged();

protected:
    void updatePosition();
    void computeContentSize();

protected:
    PositionedItem* m_parent= nullptr;
    QPointF m_position;
    qreal m_w= 0.0;
    qreal m_h= 0.0;
    bool m_isDragged= false;
    std::vector<QPointer<LinkController>> m_subNodelinks;
    bool m_open= true;
    QVector2D m_velocity{0, 0};
    QVector2D m_acceleration{0, 0};
    int m_mass= 1;
    std::map<LinkController*, QPointF> m_nextPositions;
    bool m_locked= false;
};
} // namespace mindmap
#endif // POSITIONEDITEM_H
