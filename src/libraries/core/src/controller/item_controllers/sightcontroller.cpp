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
#include "controller/item_controllers/sightcontroller.h"

#include <QDebug>
#include <QPolygonF>

//#include "controller/item_controllers/characteritemcontrollermanager.h"
#include "controller/view_controller/vectorialmapcontroller.h"

namespace vmap
{
/////////////////////////////
SightController::SightController(VectorialMapController* ctrl, QObject* parent)
    : VisualItemController(VisualItemController::SIGHT, std::map<QString, QVariant>(), ctrl, parent)
{
    // constructor
    /*connect(m_characterItems, &CharacterItemControllerManager::playableCharacterControllerCreated, this,
            &SightController::characterCountChanged);
    connect(m_characterItems, &CharacterItemControllerManager::playableCharacterControllerDestroyed, this,
            &SightController::characterCountChanged);*/

    connect(m_ctrl, &VectorialMapController::visibilityChanged, this,
            [this]() { setVisible(m_ctrl->visibility() == Core::VisibilityMode::FOGOFWAR); });
    connect(m_ctrl, &VectorialMapController::characterVisionChanged, this, &SightController::setCharacterSight);

    connect(m_ctrl, &VectorialMapController::visualRectChanged, this, &vmap::SightController::rectChanged);
    setVisible(m_ctrl->visibility() == Core::VisibilityMode::FOGOFWAR);

    connect(this, &SightController::characterSightChanged, this, [this] { setModified(); });
    connect(this, &SightController::fowPathChanged, this, [this] { setModified(); });
    connect(this, &SightController::rectChanged, this, [this] { setModified(); });
    connect(this, &SightController::characterCountChanged, this, [this] { setModified(); });
    setEditable(false);
}

void SightController::aboutToBeRemoved()
{
    emit removeItem();
}

const QList<QPointer<CharacterVision>>& SightController::visionData() const
{
    return m_visions;
}

void SightController::setCorner(const QPointF& move, int corner, Core::TransformType tt) {}

void SightController::endGeometryChange() {}

int SightController::fowItemCount() const
{
    return static_cast<int>(m_fogSingularityList.size());
}

int SightController::characterCount() const
{
    return m_visions.count();
}

bool SightController::characterSight() const
{
    return m_characterSight;
}

QRectF SightController::rect() const
{
    auto rect= m_ctrl->visualRect();
    rect= rect.united(m_rect);
    return rect;
}

void SightController::setRect(QRectF rect)
{
    if(m_rect == rect)
        return;

    m_rect= rect;
    emit rectChanged(m_rect);
}

void SightController::setCharacterSight(bool b)
{
    if(b == m_characterSight)
        return;
    m_characterSight= b;
    emit characterSightChanged();
}

QPainterPath SightController::fowPath() const
{
    QPainterPath path;
    path.addRect(rect());
    for(auto& fogs : m_fogSingularityList)
    {
        QPainterPath subPoly;
        subPoly.addPolygon(fogs.first);
        path= fogs.second ? path.united(subPoly) : path.subtracted(subPoly);
    }
    return path;
}

void SightController::addPolygon(const QPolygonF& poly, bool mask)
{
    m_fogSingularityList.push_back(std::make_pair(poly, mask));
    emit fowPathChanged(); // static_cast<int>(m_fogSingularityList.size())
}

void SightController::addCharacterVision(CharacterVision* vision)
{
    connect(vision, &CharacterVision::angleChanged, this, &SightController::characterSightChanged);
    connect(vision, &CharacterVision::radiusChanged, this, &SightController::characterSightChanged);
    connect(vision, &CharacterVision::positionChanged, this, &SightController::characterSightChanged);
    connect(vision, &CharacterVision::shapeChanged, this, &SightController::characterSightChanged);
    m_visions.push_back(vision);
    emit characterCountChanged();
}

void SightController::removeCharacterVision(CharacterVision* vision)
{
    connect(vision, 0, this, 0);
    if(m_visions.removeOne(vision))
        emit characterCountChanged();
}

const std::vector<std::pair<QPolygonF, bool>>& SightController::singularityList() const
{
    return m_fogSingularityList;
}
} // namespace vmap
