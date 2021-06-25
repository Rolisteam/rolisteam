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
#include "avatardelegate.h"

#include <QPainter>

#include "core/model/nonplayablecharactermodel.h"

constexpr int k_divisor= 2;
constexpr int k_half= 32;
constexpr int k_full= 64;

AvatarDelegate::AvatarDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

void AvatarDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if(!index.isValid())
    {
        return;
    }
    painter->save();

    auto img= qvariant_cast<QPixmap>(index.data(campaign::NonPlayableCharacterModel::RoleAvatar));

    auto x= option.rect.width() / k_divisor - k_half;
    QRectF target(x, option.rect.y(), k_full, k_full);

    painter->drawPixmap(target, img, img.rect());
    painter->restore();
}

QSize AvatarDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(k_full, k_full);
}
