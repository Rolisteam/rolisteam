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

#ifndef SIGHTITEM_H
#define SIGHTITEM_H

#include "characteritem.h"
#include "data/charactervision.h"
#include "rwidgets_global.h"
#include "visualitem.h"
#include <QConicalGradient>
#include <QGradient>
#include <QRadialGradient>
/*class FogSingularity
{
public:
    FogSingularity(QPolygonF* poly= nullptr, bool isAdding= false);
    const QPolygonF* getPolygon() const;

    bool isAdding() const;
    void fillMessage(NetworkMessageWriter* msg);
    void readItem(NetworkMessageReader* msg);
    void setPolygon(QPolygonF*);

private:
    QPolygonF* m_poly;
    bool m_adding;
};*/
namespace vmap
{
class SightController;
}
/**
 * @brief The SightItem class
 */
class RWIDGET_EXPORT SightItem : public VisualItem
{
    Q_OBJECT
public:
    /**
     * @brief SightItem
     * @param characterItemMap
     */
    explicit SightItem(vmap::SightController* ctrl);
    /**
     * @brief ~SightItem
     */
    virtual ~SightItem();
    /**
     * @brief setNewEnd
     * @param nend
     */
    virtual void setNewEnd(const QPointF& nend) override;

    /**
     * @brief getItemCopy
     * @return
     */
    virtual VisualItem* getItemCopy() override;
    /**
     * @brief boundingRect
     * @return
     */
    virtual QRectF boundingRect() const override;
    // void monitorView();
    /**
     * @brief paint
     * @param painter
     * @param option
     * @param widget
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    /**
     * @brief createActions
     */
    void createActions() override;
    virtual void updateItemFlags() override;
public slots:
    /**
     * @brief moveVision
     * @param id
     * @param pos
     */
    void moveVision(qreal id, QPointF& pos);
    /**
     * @brief removeVision vision to the given character.
     * @param item
     */
    // void removeVision(CharacterItem* item);
private:
    QPointer<vmap::SightController> m_sightCtrl;
};

#endif // SIGHTITEM_H
