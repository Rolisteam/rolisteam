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
#include "utils/mappinghelper.h"

#include <QDebug>
namespace utils
{
MappingHelper::MappingHelper(QObject* parent) : QObject{parent} {}
QSizeF MappingHelper::mapSizeTo(const QSizeF& maxSize, const QSizeF& currentSize)
{
    return currentSize.scaled(maxSize, Qt::KeepAspectRatio);
}

QRectF MappingHelper::mapRectInto(const QRectF& maxRect, const QRectF& viewRect, const QRectF& imageRect)
{
    // maxRect => viewRect
    // imageRect => res
    auto res= imageRect;

    return res;
    // return viewRect * imageRect / maxRect;
}
} // namespace utils
