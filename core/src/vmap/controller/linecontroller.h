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
#ifndef LINEITEMCONTROLLER_H
#define LINEITEMCONTROLLER_H

#include <QColor>

#include "visualitemcontroller.h"
class VectorialMapController;
namespace vmap
{
class LineController : public VisualItemController
{
    Q_OBJECT
    Q_PROPERTY(QPointF startPoint READ startPoint WRITE setStartPoint NOTIFY startPointChanged)
    Q_PROPERTY(QPointF endPoint READ endPoint WRITE setEndPoint NOTIFY endPointChanged)
    Q_PROPERTY(quint16 penWidth READ penWidth NOTIFY penWidthChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
public:
    enum TIPS
    {
        Start= 0,
        End
    };
    LineController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl, QObject* parent= nullptr);

    quint16 penWidth() const;
    QColor color() const;
    QPointF endPoint() const;
    QPointF startPoint() const;
    void aboutToBeRemoved() override;
    void endGeometryChange() override;

signals:
    void colorChanged();
    void penWidthChanged();
    void endPointChanged();
    void startPointChanged();

public slots:
    void setCorner(const QPointF& move, int corner) override;
    void setStartPoint(const QPointF& p);
    void setEndPoint(const QPointF& p);
    void setColor(const QColor& color);

private:
    QPointF m_start;
    QPointF m_end;
    QColor m_color;
    quint16 m_penWidth;
};
} // namespace vmap
#endif // LINEITEMCONTROLLER_H
