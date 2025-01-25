/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#include "mindmap/data/packagenode.h"

#include <QDebug>
namespace mindmap
{
PackageNode::PackageNode(QObject* parent) : PositionedItem{MindItem::PackageType, parent}, m_title{tr("Package")}
{
    connect(this, &PackageNode::itemDragged, this,
            [this](const QPointF& motion)
            {
                std::for_each(std::begin(m_internalChildren), std::end(m_internalChildren),
                              [motion](PositionedItem* item)
                              {
                                  if(!item)
                                      return;
                                  auto pos= item->position();
                                  item->setPosition({pos.x() - motion.x(), pos.y() - motion.y()});
                              });
            });

    connect(this, &PackageNode::widthChanged, this, &PackageNode::performLayout);
    connect(this, &PackageNode::heightChanged, this, &PackageNode::performLayout);
    connect(this, &PackageNode::visibleChanged, this,
            [this](bool visible)
            {
                std::for_each(std::begin(m_internalChildren), std::end(m_internalChildren),
                              [visible](PositionedItem* item) { item->setVisible(visible); });
            });
}

const QString& PackageNode::title() const
{
    return m_title;
}

void PackageNode::setTitle(const QString& newTitle)
{
    if(m_title == newTitle)
        return;
    m_title= newTitle;
    emit titleChanged();
}

void PackageNode::addChild(PositionedItem* item)
{
    if(m_internalChildren.contains(item) || item == this || item == nullptr)
        return;
    m_internalChildren.append(item);
    performLayout();
    emit childAdded(item->id());
}

const QList<PositionedItem*>& PackageNode::children() const
{
    return m_internalChildren;
}

void PackageNode::performLayout()
{
    if(m_internalChildren.isEmpty())
        return;
    // qDebug() << "Perform Layout";

    auto const itemCount= static_cast<std::size_t>(m_internalChildren.size());
    auto const w= width();

    std::vector<int> vec;
    std::transform(std::begin(m_internalChildren), std::end(m_internalChildren), std::back_inserter(vec),
                   [](PositionedItem* item) { return item->width(); });
    auto maxWidth= *std::max_element(std::begin(vec), std::end(vec));

    std::size_t itemPerLine= 1;
    while(maxWidth < (width() * 0.75) && itemCount > itemPerLine)
    {
        ++itemPerLine;
        int maxW= 0;
        std::vector<int> rows;
        rows.reserve(itemPerLine);
        for(auto w : vec)
        {
            if(rows.size() < itemPerLine)
            {
                rows.push_back(w);
            }
            else
            {
                maxW= std::max(std::accumulate(std::begin(rows), std::end(rows), 0), maxW);
                rows.clear();
            }
        }
        maxW= std::max(std::accumulate(std::begin(rows), std::end(rows), 0), maxW);
        rows.clear();
        maxWidth= maxW;
        qDebug() << itemPerLine << " maxwidth:" << maxWidth;
    }

    if(maxWidth > width())
    {
        --itemPerLine;
    }
    // auto const itemPerLine= std::min(static_cast<int>((w - m_minimumMargin) / (m_sizeItem + m_minimumMargin)),
    //                                static_cast<int>(itemCount));

    auto currentMarge= (w - maxWidth) / (itemPerLine + 1);

    auto currentX= itemPerLine == itemCount ? currentMarge : m_minimumMargin;
    auto currentY= m_minimumMargin;

    int i= 0;
    qreal maxH= 0.;
    for(auto item : m_internalChildren)
    {
        auto p= position();
        QPointF a{currentX + p.x(), currentY + p.y()};
        // qDebug() << a;
        item->setPosition(a);

        currentX+= item->width() + currentMarge;
        maxH= std::max(item->height(), maxH);
        ++i;
        if(i >= itemPerLine)
        {
            currentY+= maxH + m_minimumMargin;
            currentX= m_minimumMargin;
            maxH= 0.;
        }
    }
}

int PackageNode::minimumMargin() const
{
    return m_minimumMargin;
}

void PackageNode::setMinimumMargin(int newMinimumMargin)
{
    if(m_minimumMargin == newMinimumMargin)
        return;
    m_minimumMargin= newMinimumMargin;
    emit minimumMarginChanged();
}

QStringList PackageNode::childrenId() const
{
    QStringList res;

    std::transform(std::begin(m_internalChildren), std::end(m_internalChildren), std::back_inserter(res),
                   [](PositionedItem* item) { return item->id(); });

    return res;
}

} // namespace mindmap
