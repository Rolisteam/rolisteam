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
#include "boxmodel.h"

#include <QColor>
#include <QDebug>
#include <QRectF>

BoxModel::BoxModel(QObject* parent) : QAbstractItemModel(parent) {}

BoxModel::~BoxModel() {}

int BoxModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return static_cast<int>(m_data.size());

    return 0;
}

int BoxModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return 1;
}

QModelIndex BoxModel::parent(const QModelIndex& index) const
{
    return QModelIndex();
}

QVariant BoxModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    QVariant result;
    auto mindNode= static_cast<MindNode*>(index.internalPointer());
    switch(role)
    {
    case Color:
        result= QVariant::fromValue(QColor(Qt::gray));
        break;
    case ContentWidth:
        result= mindNode->contentWidth();
        break;
    case ContentHeight:
        result= mindNode->contentHeight();
        break;
    case Label:
        result= mindNode->text();
        break;
    case Position:
        result= mindNode->position();
        break;
    case Posx:
        result= mindNode->position().x();
        break;
    case Posy:
        result= mindNode->position().y();
        break;
    case Node:
        result= QVariant::fromValue(mindNode);
        break;
    }
    return result;
}

bool BoxModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(data(index, role) != value)
    {
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

QModelIndex BoxModel::index(int row, int column, const QModelIndex& parent) const
{
    if(row < 0)
        return QModelIndex();

    MindNode* childItem= nullptr;

    if(!parent.isValid())
        childItem= m_data.at(row);

    if(childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QAbstractItemModel* BoxModel::linkModel() const
{
    return m_linkModel;
}

void BoxModel::setLinkModel(QAbstractItemModel* model)
{
    auto linkModel= dynamic_cast<LinkModel*>(model);
    if(linkModel == m_linkModel)
        return;

    m_linkModel= linkModel;
    emit linkModelChanged();
}

Qt::ItemFlags BoxModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

QHash<int, QByteArray> BoxModel::roleNames() const
{
    static QHash<int, QByteArray> roles= {{BoxModel::Label, "label"},
                                          {BoxModel::ContentWidth, "contentWidth"},
                                          {BoxModel::ContentHeight, "contentHeight"},
                                          {BoxModel::Color, "colorbg"},
                                          {BoxModel::Position, "position"},
                                          {BoxModel::Node, "node"},
                                          {BoxModel::Posx, "posx"},
                                          {BoxModel::Posy, "posy"}};
    return roles;
}

void BoxModel::clear()
{
    auto backup= m_data;
    beginResetModel();
    m_data.clear();
    endResetModel();
    qDeleteAll(backup);

    m_nodeHeight= 0.;
    m_nodeWidth= 0.;
}

void BoxModel::appendNode(MindNode* node)
{
    if(node == nullptr)
        return;
    auto row= static_cast<int>(m_data.size());
    beginInsertRows(QModelIndex(), row, row);
    m_data.push_back(node);
    endInsertRows();
    computeContentSize(node);
}

std::pair<MindNode*, Link*> BoxModel::addBox(const QString& idparent)
{
    auto row= static_cast<int>(m_data.size());
    beginInsertRows(QModelIndex(), row, row);
    auto root= new MindNode();
    root->setText(tr("New Node"));
    root->setStyleIndex(m_defaultStyleIndex);

    m_data.push_back(root);
    if(idparent.isEmpty())
        return {};

    auto id= std::find_if(m_data.begin(), m_data.end(),
                          [idparent](const MindNode* node) { return idparent == node->id(); });
    if(id == m_data.end())
        return {};

    auto rectParent= (*id)->boundingRect();
    auto pos= rectParent.topLeft() + QPointF(rectParent.width() * 1.5, rectParent.height() * 1.5);
    root->setPosition(pos);
    endInsertRows();

    computeContentSize(root);

    auto link= m_linkModel->addLink(*id, root);

    return std::make_pair(root, link);
}

bool BoxModel::removeBox(const MindNode* node)
{
    if(node == nullptr)
        return false;
    auto it= std::find(m_data.begin(), m_data.end(), node);
    auto idx= static_cast<int>(std::distance(m_data.begin(), it));

    beginRemoveRows(QModelIndex(), idx, idx);
    m_data.erase(it);
    endRemoveRows();
    return true;
}

void BoxModel::openNode(const QString& id, bool status)
{
    auto it= node(id);

    if(nullptr == it)
        return;

    if(it->open() == status)
        return;

    it->setOpen(status);
}

MindNode* BoxModel::node(const QString& id) const
{
    auto it= std::find_if(m_data.begin(), m_data.end(), [id](const MindNode* node) { return node->id() == id; });

    if(it == m_data.end())
        return nullptr;

    return (*it);
}

std::vector<MindNode*>& BoxModel::nodes()
{
    return m_data;
}

void BoxModel::setDefaultStyleIndex(int indx)
{
    if(indx == m_defaultStyleIndex)
        return;
    m_defaultStyleIndex= indx;
    emit defaultStyleIndexChanged();
}

void BoxModel::setNodeWidth(qreal w)
{
    if(qFuzzyCompare(w, m_nodeWidth))
        return;
    m_nodeWidth= w;
    emit nodeWidthChanged();
}

void BoxModel::setNodeHeight(qreal h)
{
    if(qFuzzyCompare(h, m_nodeHeight))
        return;
    m_nodeHeight= h;
    emit nodeHeightChanged();
}

void BoxModel::computeContentSize(const MindNode* newNode)
{
    setNodeWidth(std::max(newNode->position().x() + newNode->contentWidth(), m_nodeWidth));
    setNodeHeight(std::max(newNode->position().y() + newNode->contentHeight(), m_nodeHeight));
}

int BoxModel::defaultStyleIndex() const
{
    return m_defaultStyleIndex;
}

qreal BoxModel::nodeWidth() const
{
    return m_nodeWidth;
}

qreal BoxModel::nodeHeight() const
{
    return m_nodeHeight;
}
