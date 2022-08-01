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
#ifndef RECTITEMCONTROLLER_H
#define RECTITEMCONTROLLER_H

#include <QColor>
#include <QRectF>
#include <QVariant>

#include "visualitemcontroller.h"
#include <core_global.h>
class VectorialMapController;
namespace vmap
{
class CORE_EXPORT RectController : public VisualItemController
{
    Q_OBJECT
    Q_PROPERTY(QRectF rect READ rect WRITE setRect NOTIFY rectChanged)
    Q_PROPERTY(bool filled READ filled NOTIFY filledChanged)
    Q_PROPERTY(quint16 penWidth READ penWidth NOTIFY penWidthChanged)
public:
    RectController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl, QObject* parent= nullptr);

    bool filled() const;
    QRectF rect() const override;
    quint16 penWidth() const;

    void aboutToBeRemoved() override;
    void endGeometryChange() override;

signals:
    void filledChanged();
    void rectChanged();
    void rectEditFinished();
    void penWidthChanged();

public slots:
    void setCorner(const QPointF& move, int corner, Core::TransformType tt= Core::TransformType::NoTransform) override;
    void setRect(QRectF rect);

private:
    QRectF m_rect= QRectF(0, 0, 1, 1);
    bool m_filled;
    quint16 m_penWidth;

    bool m_rectEdited= false;
};
} // namespace vmap

#endif // RECTITEMCONTROLLER_H
