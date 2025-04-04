/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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

#ifndef RULEITEM_H
#define RULEITEM_H

#include "rwidgets_global.h"

#include <QGraphicsObject>
#include <QPen>
#include <QPointer>

class VectorialMapController;
/**
 * @brief The RuleItem class ephemeral item to display rule and measure the distance between two points.
 */
class RWIDGET_EXPORT RuleItem : public QGraphicsObject
{
    Q_OBJECT
public:
    RuleItem(VectorialMapController* ctrl);
    ~RuleItem();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr);
    virtual QRectF boundingRect() const;
    virtual void setNewEnd(const QPointF& nend, bool onAxis);

private:
    QPointer<VectorialMapController> m_ctrl;
    QPointF m_startPoint= QPointF(0, 0);
    QPointF m_endPoint= QPointF(0, 0);
    QPen m_pen= QColor(Qt::red);
};

#endif // RULEITEM_H
