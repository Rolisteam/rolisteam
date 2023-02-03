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

#ifndef GRIDITEM_H
#define GRIDITEM_H

#include <QConicalGradient>
#include <QGradient>
#include <QPointer>
#include <QRadialGradient>

#include "characteritem.h"
#include "controller/item_controllers/gridcontroller.h"
#include "data/charactervision.h"
#include "rwidgets_global.h"
#include "visualitem.h"
namespace vmap
{

class VisualItemController;
}
/**
 * @brief The SightItem class
 */
class RWIDGET_EXPORT GridItem : public VisualItem
{
    Q_OBJECT
public:
    /**
     * @brief GridItem
     * @param characterItemMap
     */
    GridItem(vmap::GridController* ctrl);
    /**
     * @brief ~GridItem
     */
    virtual ~GridItem();
    void setNewEnd(const QPointF& nend) override;
    /**
     * @brief boundingRect
     * @return
     */
    virtual QRectF boundingRect() const override;
    /**
     * @brief paint
     * @param painter
     * @param option
     * @param widget
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    virtual void updateItemFlags() override;

private:
    QPointer<vmap::GridController> m_gridCtrl;
};

#endif // GRIDITEM_H
