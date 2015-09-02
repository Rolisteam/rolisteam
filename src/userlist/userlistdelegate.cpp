/***************************************************************************
    *     Copyright (C) 2009 by Renaud Guezennec                             *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify     *
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
#include "userlistdelegate.h"
#include <QPainter>
#include <QDebug>

#include "playersList.h"
#include "data/person.h"

UserListDelegate::UserListDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}
void UserListDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{

    if(!index.isValid())
    {
        return;
    }
    QString uuid = index.data(PlayersList::IdentifierRole).toString();
    Person* p = PlayersList::instance()->getPerson(uuid);
    painter->save();
    if(p!=NULL)
    {
        QRect tmp = option.rect;
        if (option.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, option.palette.highlight());
        }

        painter->fillRect(option.rect.x(),option.rect.y(),option.decorationSize.width(),option.rect.height(),p->getColor());
        QRectF target(option.rect.x(), option.rect.y(),option.decorationSize.width(), option.rect.height());
        painter->drawImage(target,p->getAvatar(),p->getAvatar().rect());
        tmp.adjust(option.decorationSize.width(),1,1,1);
        painter->drawText(tmp,Qt::AlignVCenter,p->getName());
    }
    painter->restore();
}
QSize UserListDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QSize returnValue;
    QString uuid = index.data(PlayersList::IdentifierRole).toString();
    Person* p = PlayersList::instance()->getPerson(uuid);
    if(p!=NULL)
    {
        returnValue.setWidth(option.fontMetrics.width(p->getName()));
        returnValue.setHeight(/*option.fontMetrics.height()*/option.decorationSize.height());
    }

    return returnValue;
}
