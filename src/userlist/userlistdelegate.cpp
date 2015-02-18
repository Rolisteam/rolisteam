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
#include "userlistmodel.h"
#include <QPainter>
#include <QDebug>
UserListDelegate::UserListDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}
void UserListDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    PersonItem* p = static_cast<PersonItem*>(index.internalPointer());
    painter->save();
    if(p!=NULL)
    {
        QRect tmp = option.rect;
        //tmp.setLeft();
        if (option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());

        //QRectF rectangle(option.rect.x(),option.rect.y(),option.decorationSize.width(),option.rect.height());
        //painter->setPen(Qt::NoPen);
        //QBrush brush(p->getPerson()->getColor());
        //painter->setBrush(brush);
        painter->fillRect(option.rect.x(),option.rect.y(),option.decorationSize.width(),option.rect.height(),p->getPerson()->getColor());
        //painter->drawRoundedRect(rectangle, 15.0, 15.0);

        QRectF target(option.rect.x(), option.rect.y(),option.decorationSize.width(), option.rect.height());



        painter->drawImage(target,p->getPerson()->getAvatar(),p->getPerson()->getAvatar().rect());
        tmp.adjust(option.decorationSize.width(),1,1,1);
        painter->drawText(tmp,Qt::AlignVCenter,p->getPerson()->getName());


    }
    painter->restore();
    //qDebug() << option.decorationSize << option.rect << p->getPerson()->getColor()<< option.decorationPosition;
    //QItemDelegate::paint(painter,option,index);
    //QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, box, painter);

}
QSize UserListDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QSize returnValue;
    PersonItem* p = static_cast<PersonItem*>(index.internalPointer());
    if(p!=NULL)
    {
        returnValue.setWidth(option.fontMetrics.width(p->getPerson()->getName()));
        //if(option.decorationSize.height())
        returnValue.setHeight(/*option.fontMetrics.height()*/option.decorationSize.height());
    }
  //  qDebug() << " height" << returnValue.height();
   // qDebug() << QItemDelegate::sizeHint(option,index);
    return returnValue;
   // return QItemDelegate::sizeHint(option,index);
}
