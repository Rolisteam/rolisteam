/***************************************************************************
 *      Copyright (C) 2010 by Renaud Guezennec                             *
 *                                                                         *
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
#ifndef CHARACTERITEM_H
#define CHARACTERITEM_H

#include <QAction>
#include <QPointer>

#include "data/characterstate.h"
#include "data/charactervision.h"
#include "data/person.h"
#include "diceparser/diceparser.h"
#include "rwidgets/rwidgets_global.h"
#include "visualitem.h"
#include <memory>
namespace vmap
{
class CharacterItemController;
}
/**
 * @brief represents any character on map.
 */
class RWIDGET_EXPORT CharacterItem : public VisualItem
{
    Q_OBJECT
public:
    enum Corner
    {
        TopLeft,
        TopRight,
        BottomRight,
        BottomLeft,
        SightAngle,
        SightLenght
    };
    Q_ENUM(Corner)
    /**
     * @brief CharacterItem
     */
    CharacterItem(vmap::CharacterItemController* ctrl);
    ~CharacterItem() override;

    // Override
    virtual QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr) override;
    void updateChildPosition() override;
    virtual void addActionContextMenu(QMenu&) override;
    void setNewEnd(const QPointF& p) override;

    // accessors
    void addChildPoint(ChildPointItem* item);
    void sendVisionMsg();
    void updateCharacter();

signals:
    void positionChanged();
    void geometryChangeOnUnkownChild(qreal pointId, QPointF& F);
    void localItemZValueChange(CharacterItem*);
    void ownerChanged(Character* old, CharacterItem*);
    void runDiceCommand(QString cmd, QString uuid);

protected:
    virtual void wheelEvent(QGraphicsSceneWheelEvent* event) override;

private slots:
    void changeCharacter();

private:
    QPointer<vmap::CharacterItemController> m_itemCtrl;
    QPointer<VectorialMapController> m_mapCtrl;

    // QAction
    std::unique_ptr<QAction> m_visionShapeDisk;
    std::unique_ptr<QAction> m_visionShapeAngle;
    std::unique_ptr<QAction> m_reduceLife;
    std::unique_ptr<QAction> m_increaseLife;
};

#endif // CHARACTERITEM_H
