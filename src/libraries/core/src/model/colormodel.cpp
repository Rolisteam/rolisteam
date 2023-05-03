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
#include "model/colormodel.h"
#include "model/nonplayablecharactermodel.h"
#include <QSet>

ColorModel::ColorModel(QObject* parent) : QAbstractListModel(parent) {}

int ColorModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if(parent.isValid())
        return 0;

    return m_data.size() + 1;
}

QVariant ColorModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto i= index.row();

    if(i == 0 && role == Qt::DisplayRole)
        return tr("Any color");
    else
        return {};

    auto color= m_data[i - 1];

    if(role == Qt::DecorationRole)
        return color;
    else // if(role == Qt::DisplayRole)
        return color.name();
}

void ColorModel::setSourceModel(const campaign::NonPlayableCharacterModel* source)
{

    connect(source, &campaign::NonPlayableCharacterModel::dataChanged, this,
            [this, &source](const QModelIndex& start, const QModelIndex& end, QList<int> roles)
            {
                if(roles.contains(campaign::NonPlayableCharacterModel::RoleColor))
                {
                    auto index= start;
                    for(int i= start.row(); i < end.row(); ++i)
                    {
                        auto color= source->data(index, campaign::NonPlayableCharacterModel::RoleColor).value<QColor>();

                        if(m_data.contains(color))
                            return;

                        beginInsertRows(QModelIndex(), rowCount(), rowCount());
                        m_data.append(color);
                        endInsertRows();
                    }
                }
            });
    connect(source, &campaign::NonPlayableCharacterModel::rowsInserted, this,
            [this, source](const QModelIndex& parent, int start, int end)
            {
                for(int i= start; i < end; ++i)
                {
                    if(i < source->rowCount())
                    {
                        auto color
                            = source->data(source->index(i, 0, parent), campaign::NonPlayableCharacterModel::RoleColor)
                                  .value<QColor>();

                        if(m_data.contains(color))
                            return;

                        beginInsertRows(QModelIndex(), rowCount(), rowCount());
                        m_data.append(color);
                        endInsertRows();
                    }
                }
            });

    QSet<QColor> colors;
    auto const& npcs= source->npcList();
    std::for_each(std::begin(npcs), std::end(npcs),
                  [&colors](const std::unique_ptr<campaign::NonPlayableCharacter>& npc)
                  {
                      colors.insert(npc->getColor());
                  });

    beginResetModel();
    m_data= colors.values();
    endResetModel();
}
