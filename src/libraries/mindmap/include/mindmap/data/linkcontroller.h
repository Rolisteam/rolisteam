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
#ifndef LINKCONTROLLER_H
#define LINKCONTROLLER_H

#include <QColor>
#include <QObject>
#include <QPointF>
#include <QPointer>
#include <QRectF>
#include <Qt>

#include "mindmap/data/minditem.h"
#include "mindmap/data/positioneditem.h"

namespace mindmap
{
class LinkController : public MindItem
{
    Q_OBJECT
    Q_PROPERTY(Direction direction READ direction WRITE setDirection NOTIFY directionChanged)

    Q_PROPERTY(QPointF topLeftCorner READ topLeftCorner NOTIFY normalizedRectChanged)
    Q_PROPERTY(qreal normalizedWidth READ normalizedWidth NOTIFY normalizedRectChanged)
    Q_PROPERTY(qreal normalizedHeight READ normalizedHeight NOTIFY normalizedRectChanged)

    Q_PROPERTY(mindmap::PositionedItem* start READ start WRITE setStart NOTIFY startChanged)
    Q_PROPERTY(mindmap::PositionedItem* end READ end WRITE setEnd NOTIFY endChanged)

    Q_PROPERTY(QPointF startPoint READ startPoint NOTIFY startPointChanged)
    Q_PROPERTY(QPointF endPoint READ endPoint NOTIFY endPointChanged)

    Q_PROPERTY(QRectF startBox READ startBox NOTIFY startBoxChanged)
    Q_PROPERTY(QRectF endBox READ endBox NOTIFY endBoxChanged)

    Q_PROPERTY(qreal width READ width NOTIFY geometryChanged)
    Q_PROPERTY(qreal height READ height NOTIFY geometryChanged)

    Q_PROPERTY(bool color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(Qt::PenStyle lineStyle READ lineStyle WRITE setLineStyle NOTIFY lineStyleChanged)
    Q_PROPERTY(bool constraint READ constraint WRITE setConstraint NOTIFY constraintChanged)
    Q_PROPERTY(Orientation orientation READ orientation NOTIFY orientationChanged)
public:
    enum Direction
    {
        StartToEnd,
        EndToStart,
        Both
    };
    Q_ENUM(Direction)
    enum Orientation
    {
        RightBottom,
        LeftBottom,
        RightTop,
        LeftTop
    };
    Q_ENUM(Orientation)
    explicit LinkController(QObject* parent= nullptr);

    Direction direction() const;
    mindmap::PositionedItem* start() const;
    mindmap::PositionedItem* end() const;
    QPointF endPoint() const;
    QPointF startPoint() const;

    QPointF topLeftCorner() const;
    qreal normalizedWidth() const;
    qreal normalizedHeight() const;

    LinkController::Orientation orientation() const;

    float getLength() const;
    float getStiffness() const;
    void cleanUpLink();

    const QRectF endBox() const;
    const QRectF startBox() const;

    qreal width() const;
    qreal height() const;

    bool color() const;
    void setColor(const bool& newColor);

    const Qt::PenStyle& lineStyle() const;
    void setLineStyle(const Qt::PenStyle& newLineStyle);

    bool constraint() const;
    void setConstraint(bool newConstraint);
    QString toString(bool b) const;

public slots:
    void setStiffness(float stiffness);
    void setDirection(const LinkController::Direction& direction);
    void setEnd(mindmap::PositionedItem* end);
    void setStart(mindmap::PositionedItem* start);
    void computePosition();

signals:
    void sizeChanged();
    void directionChanged();
    void startChanged();
    void endChanged();
    void startPointChanged();
    void endPointChanged();
    void startBoxChanged();
    void endBoxChanged();
    void geometryChanged();
    void colorChanged();
    void lineStyleChanged();
    void constraintChanged();
    void normalizedRectChanged();
    void orientationChanged();

private slots:
    void setNormalizedRect(QRectF rect);

private:
    void computeNormalizedRect();

private:
    Direction m_dir= StartToEnd;
    QPointer<mindmap::PositionedItem> m_start;
    QPointer<mindmap::PositionedItem> m_end;
    float m_stiffness= 400.0f;
    bool m_color= true;
    Qt::PenStyle m_lineStyle= Qt::SolidLine;
    bool m_constraint= true;
    QRectF m_normalizedRect;
    Orientation m_orient;
};
} // namespace mindmap
#endif // LINKCONTROLLER_H
