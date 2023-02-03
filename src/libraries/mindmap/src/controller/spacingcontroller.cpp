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
#include "mindmap/controller/spacingcontroller.h"

#include "mindmap/data/linkcontroller.h"
#include "mindmap/model/minditemmodel.h"

#include <QDebug>
#include <QLineF>
#include <chrono>
#include <cmath>
#include <thread>

namespace mindmap
{
constexpr float k_attraction{0.1f};
constexpr float k_repulsion{10000.f};
constexpr float k_defaultDamping{0.5f};

SpacingController::SpacingController(MindItemModel* data, QObject* parent) : QObject(parent), m_model(data) {}

SpacingController::~SpacingController()= default;

void SpacingController::setRunning(bool run)
{
    if(run == m_running)
        return;
    m_running= run;
    emit runningChanged();
}

bool SpacingController::running() const
{
    return m_running;
}

void SpacingController::computeInLoop()
{
    while(m_running)
    {
        auto const& packages= m_model->items(MindItem::PackageType);
        QList<PositionedItem*> packagedChildren;
        for(auto item : packages)
        {
            auto pack= dynamic_cast<PackageNode*>(item);
            packagedChildren.append(pack->children());
        }

        auto const& items= m_model->items(MindItem::NodeType);

        std::vector<PositionedItem*> allNodes;
        allNodes.reserve(items.size());
        std::transform(std::begin(items), std::end(items), std::back_inserter(allNodes),
                       [](MindItem* item) { return dynamic_cast<PositionedItem*>(item); });

        allNodes.erase(std::remove_if(std::begin(allNodes), std::end(allNodes),
                                      [packagedChildren](PositionedItem* item)
                                      { return packagedChildren.contains(item); }),
                       std::end(allNodes));

        for(auto& node : allNodes)
        {
            applyCoulombsLaw(node, allNodes);
        }
        auto const allLinks= m_model->items(MindItem::LinkType);
        for(auto& item : allLinks)
        {
            auto link= dynamic_cast<LinkController*>(item);
            if(!link)
                continue;

            if(!link->constraint())
                continue;

            applyHookesLaw(link);
        }
        for(auto& node : allNodes)
        {
            node->setVelocity(node->getVelocity() * k_defaultDamping);
            if(!node->isDragged())
                node->setPosition(node->position() + node->getVelocity().toPointF());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    Q_EMIT finished();
}

void SpacingController::applyCoulombsLaw(PositionedItem* node, std::vector<PositionedItem*> nodeList)
{
    auto globalRepulsionForce= QVector2D();
    for(auto const& otherNode : nodeList)
    {
        if(node == otherNode)
            continue;

        auto vect= QVector2D(node->position() - otherNode->position());
        auto length= vect.length();
        auto force= k_repulsion / std::pow(length, 2);

        globalRepulsionForce+= vect.normalized() * force;
    }

    node->setVelocity(node->getVelocity() + globalRepulsionForce);
}

void SpacingController::applyHookesLaw(LinkController* link)
{
    if(!link)
        return;

    auto node1= link->start();
    auto node2= link->end();

    if(node1 == nullptr || node2 == nullptr)
        return;

    auto p1= node1->position();
    auto p2= node2->position();

    if(qIsNaN(p1.x()) && qIsNaN(p1.y()))
    {
        p1= {0, 0};
    }
    if(qIsNaN(p2.x()) && qIsNaN(p2.y()))
    {
        p2= {1, 1};
    }

    auto vect= QVector2D(p1 - p2);
    auto length= vect.length();
    // auto force= k_attraction * std::max(length - k_defaultSpringLength, 0.f);
    auto force= k_attraction * std::max(length - link->getLength(), 0.1f);

    node1->setVelocity(node1->getVelocity() + vect.normalized() * force * -1);
    node2->setVelocity(node2->getVelocity() + vect.normalized() * force);
}
} // namespace mindmap
