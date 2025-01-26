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
#include "mindmap/data/mindnode.h"

#include <QDebug>
#include <QFontMetricsF>
#include <QRectF>
#include <QUuid>

//"link.h"
namespace mindmap
{

MindNode::MindNode(QObject* parent) : PositionedItem(MindItem::NodeType, parent)
{
    setText(tr("New node"));
}
MindNode::~MindNode()= default;

int MindNode::styleIndex() const
{
    return m_styleIndex;
}

QString MindNode::description() const
{
    return m_description;
}

void MindNode::setStyleIndex(int idx)
{
    if(idx == m_styleIndex)
        return;
    m_styleIndex= idx;
    emit styleIndexChanged();
}

QString MindNode::imageUri() const
{
    return m_imageUri;
}

void MindNode::setImageUri(const QString& uri)
{
    if(uri == m_imageUri)
        return;
    m_imageUri= uri;
    emit imageUriChanged();
}

void MindNode::setDescription(const QString& newDescription)
{
    if(m_description == newDescription)
        return;
    m_description= newDescription;
    emit descriptionChanged();
}

const QStringList& MindNode::tags() const
{
    return m_tags;
}

void MindNode::setTags(const QStringList& newTags)
{
    if(m_tags == newTags)
        return;
    m_tags= newTags;
    emit tagsChanged();
}

const QString MindNode::tagsText() const
{
    return m_tags.join(", ");
}

void MindNode::setTagsText(const QString& newTagsText)
{
    auto list= newTagsText.split(",");
    QStringList trimmed;
    std::transform(std::begin(list), std::end(list), std::back_inserter(trimmed),
                   [](const QString& text) { return text.trimmed(); });
    setTags(trimmed);
}
} // namespace mindmap
