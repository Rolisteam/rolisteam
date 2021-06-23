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
#include "utilshelper.h"

#include <QImageReader>

namespace helper
{
namespace utils
{
QString allSupportedImageFormatFilter()
{
    auto allFormats= QImageReader::supportedImageFormats();

    QStringList list;
    std::transform(std::begin(allFormats), std::end(allFormats), std::back_inserter(list),
                   [](const QByteArray& array) { return QString("*.%1").arg(QString::fromLocal8Bit(array)); });

    return list.join(" ");
}

QRectF computerBiggerRectInside(const QRect& rect, qreal ratio)
{
    QRectF res(rect);
    auto const r= ratio;

    auto const reversed= 1 / r;

    auto future_W= res.height() * r;
    auto const future_H= res.width() * reversed;

    if(future_W < future_H && future_W <= rect.width())
    {
        res.setWidth(future_W);
    }
    else
    {
        if(future_H <= res.height())
            res.setHeight(future_H);
        else
        {
            res.setWidth(future_W);
        }
    }

    return res;
}
} // namespace utils
} // namespace helper
