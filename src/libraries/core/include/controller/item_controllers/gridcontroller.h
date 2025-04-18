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
#ifndef GRIDCONTROLLER_H
#define GRIDCONTROLLER_H

#include "visualitemcontroller.h"

#include <QImage>
#include <core_global.h>
class VectorialMapController;
namespace vmap
{
class CORE_EXPORT GridController : public VisualItemController
{
    Q_OBJECT
    Q_PROPERTY(QImage gridPattern READ gridPattern WRITE setGridPattern NOTIFY gridPatternChanged)
    Q_PROPERTY(bool gm READ gm WRITE setGm NOTIFY gmChanged)
    Q_PROPERTY(QRectF rect READ rect WRITE setRect NOTIFY rectChanged)
public:
    GridController(VectorialMapController* ctrl, QObject* parent= nullptr);

    bool gm() const;
    QImage gridPattern() const;
    QRectF rect() const override;

    void aboutToBeRemoved() override;
    void setCorner(const QPointF& move, int corner, Core::TransformType tt= Core::TransformType::NoTransform) override;
    void endGeometryChange() override;

signals:
    void gmChanged(bool gm);
    void gridPatternChanged(const QImage& gridPattern);
    void rectChanged(const QRectF& rect);

public slots:
    void setGm(bool gm);
    void setGridPattern(const QImage& gridPattern);
    void setRect(const QRectF &rect);

private:
    void computePattern();

private:
    bool m_gm{true};
    QImage m_gridPattern;
    QRectF m_rect{QRectF(0, 0, 1000, 1000)};
};
} // namespace vmap

#endif // GRIDCONTROLLER_H
