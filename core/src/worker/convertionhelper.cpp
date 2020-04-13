/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#include "convertionhelper.h"

namespace Helper
{
template <>
void variantToType<bool>(const bool& val, NetworkMessageWriter& msg)
{
    msg.uint8(val);
}

template <>
void variantToType<qreal>(const qreal& val, NetworkMessageWriter& msg)
{
    msg.real(val);
}

template <>
void variantToType<int>(const int& val, NetworkMessageWriter& msg)
{
    msg.int64(val);
}

template <>
void variantToType<Core::ScaleUnit>(const Core::ScaleUnit& val, NetworkMessageWriter& msg)
{
    msg.uint8(val);
}
template <>
void variantToType<Core::PermissionMode>(const Core::PermissionMode& val, NetworkMessageWriter& msg)
{
    msg.uint8(val);
}
template <>
void variantToType<Core::GridPattern>(const Core::GridPattern& val, NetworkMessageWriter& msg)
{
    msg.uint8(val);
}
template <>
void variantToType<Core::Layer>(const Core::Layer& val, NetworkMessageWriter& msg)
{
    msg.uint8(static_cast<typename std::underlying_type<Core::Layer>::type>(val));
}
template <>
void variantToType<Core::VisibilityMode>(const Core::VisibilityMode& val, NetworkMessageWriter& msg)
{
    msg.uint8(val);
}

template <>
void variantToType<QColor>(const QColor& val, NetworkMessageWriter& msg)
{
    msg.rgb(val.rgb());
}

template <>
void variantToType<QImage>(const QImage& val, NetworkMessageWriter& msg)
{
    QByteArray data;
    QDataStream in(&data, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_7);
    in << val;
    msg.byteArray32(data);
}

template <>
void variantToType<QPointF>(const QPointF& val, NetworkMessageWriter& msg)
{
    msg.real(val.x());
    msg.real(val.y());
}

template <>
void variantToType<QRectF>(const QRectF& val, NetworkMessageWriter& msg)
{
    msg.real(val.x());
    msg.real(val.y());
    msg.real(val.width());
    msg.real(val.height());
}
template <>
void variantToType<quint16>(const quint16& val, NetworkMessageWriter& msg)
{
    msg.uint16(val);
}

template <>
void variantToType<QByteArray>(const QByteArray& val, NetworkMessageWriter& msg)
{
    msg.byteArray32(val);
}

template <>
void variantToType<QFont>(const QFont& val, NetworkMessageWriter& msg)
{
    msg.string16(val.toString());
}
template <>
void variantToType<std::vector<QPointF>>(const std::vector<QPointF>& val, NetworkMessageWriter& msg)
{
    msg.int64(static_cast<qint64>(val.size()));
    for(auto p : val)
    {
        msg.real(p.x());
        msg.real(p.y());
    }
}
template <>
void variantToType<CharacterVision::SHAPE>(const CharacterVision::SHAPE& shape, NetworkMessageWriter& msg)
{
    msg.uint8(static_cast<quint8>(shape));
}
} // namespace Helper
