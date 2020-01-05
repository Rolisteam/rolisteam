/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#ifndef ELLIPSEITEMCONTROLLER_H
#define ELLIPSEITEMCONTROLLER_H

#include <QColor>
#include <QRectF>

#include "visualitemcontroller.h"

class VectorialMapController;
namespace vmap
{
class EllipseController : public VisualItemController
{
    Q_OBJECT
    Q_PROPERTY(bool filled READ filled NOTIFY filledChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(quint16 penWidth READ penWidth NOTIFY penWidthChanged)
    Q_PROPERTY(qreal rx READ rx WRITE setRx NOTIFY rxChanged)
    Q_PROPERTY(qreal ry READ ry WRITE setRy NOTIFY ryChanged)
public:
    EllipseController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl,
                      QObject* parent= nullptr);

    bool filled() const;
    QColor color() const;
    quint16 penWidth() const;
    qreal rx() const;
    qreal ry() const;

    void aboutToBeRemoved() override;
    void endGeometryChange() override;
    QRectF rect() const override;

signals:
    void colorChanged();
    void filledChanged();
    void penWidthChanged();
    void rxChanged();
    void ryChanged();

public slots:
    void setColor(QColor color);
    void setCorner(const QPointF& move, int corner) override;
    void setRx(qreal rx);
    void setRy(qreal ry);

private:
    bool m_filled;
    QColor m_color;
    quint16 m_penWidth;
    qreal m_rx= 0.0;
    qreal m_ry= 0.0;
};
} // namespace vmap
#endif // ELLIPSEITEMCONTROLLER_H
