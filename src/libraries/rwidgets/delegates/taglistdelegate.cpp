/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#include "taglistdelegate.h"

#include <QDebug>
#include <QFontMetrics>
#include <QPainter>

TagListDelegate::TagListDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

TagListDelegate::~TagListDelegate() {}

void TagListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    auto tags= index.data().toStringList();

    int margex= 4;
    int margey= 4;

    // qDebug() << option.palette.highlightedText() << option.palette.brightText();

    if(option.state & QStyle::State_Selected)
    {
        painter->setPen(option.palette.highlightedText().color());
    }
    else
    {
        painter->setPen(option.palette.text().color());
    }

    int startx= option.rect.x() + margex;
    int starty= option.rect.y() + margey;
    auto fontmetrics= option.fontMetrics;
    for(auto const& tag : std::as_const(tags))
    {
        auto rect= fontmetrics.boundingRect(tag);

        if(startx + rect.width() > option.rect.x() + option.rect.width())
        {
            startx= option.rect.x() + margex;
            starty+= rect.height() + margey;
        }

        rect.translate(startx, starty - rect.y());
        painter->drawText(rect, tag);
        painter->drawRoundedRect(rect, 2, 2);
        startx+= rect.width() + margex;
    }
}

QSize TagListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize ret= QStyledItemDelegate::sizeHint(option, index);
    ret= ret * 1.3; // add some more padding between items
    return ret;
}
