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
#include "nodestylemodel.h"

#include "data/nodestyle.h"

NodeStyleModel::NodeStyleModel(QObject* parent) : QAbstractListModel(parent)
{
    initStyles();
}

int NodeStyleModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if(parent.isValid())
        return 0;
    return static_cast<int>(m_styles.size());
}

QVariant NodeStyleModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row() >= static_cast<int>(m_styles.size()))
        return {};

    auto style= m_styles[static_cast<std::size_t>(index.row())];

    QVariant var;
    switch(role)
    {
    case ColorOne:
        var= QVariant::fromValue(style->colorOne());
        break;
    case ColorTwo:
        var= QVariant::fromValue(style->colorTwo());
        break;
    case TextColor:
        var= QVariant::fromValue(style->textColor());
        break;
    default:
        break;
    }

    return var;
}

QHash<int, QByteArray> NodeStyleModel::roleNames() const
{
    // clang-format off
    static QHash<int, QByteArray> roles
        =  {{NodeStyleModel::ColorOne, "colorOne"},
            {NodeStyleModel::ColorTwo, "colorTwo"},
            {NodeStyleModel::TextColor, "textColor"}};
    //clang-format on
    return roles;
}

NodeStyle* NodeStyleModel::getStyle(int index) const
{
    if(m_styles.empty())
    {
        auto tmp = new NodeStyle();
        tmp->setColorOne(Qt::white);
        tmp->setColorTwo(Qt::lightGray);
        tmp->setTextColor(Qt::black);
        return tmp;
    }
    index = std::max(0,index);

    auto idx = static_cast<std::size_t>(index);

    if(idx >= m_styles.size())
       index = 0;

    return m_styles[idx];
}

void NodeStyleModel::initStyles()
{
    std::vector<QString> colors({"#c0c0c0","#A4C400",
    "#60A917","#008A00","#00ABA9","#1BA1E2","#3E65FF",
    "#6A00FF","#AA00FF","#F472D0","#D80073","#A20025",
    "#E51400","#FA6800","#F0A30A","#E3C800",
    "#825A2C","#6D8764","#647687","#76608A"});

    beginResetModel();
    m_styles.clear();

    std::transform(colors.begin(), colors.end(), std::back_inserter(m_styles),[this](const QString& colorstr){
        QColor one(Qt::white);
        QColor text(Qt::black);
        QColor color;
        color.setNamedColor(colorstr);
        auto style = new NodeStyle(this);
        style->setColorOne(one);
        style->setColorTwo(color);
        style->setTextColor(text);
        return style;
    });

    std::transform(colors.begin(), colors.end(), std::back_inserter(m_styles),[this](const QString& colorstr){
        QColor one(Qt::black);
        QColor text(Qt::white);
        QColor color;
        color.setNamedColor(colorstr);
        auto style = new NodeStyle(this);
        style->setColorOne(color);
        style->setColorTwo(one);
        style->setTextColor(text);
        return style;
    });

    endResetModel();


}
