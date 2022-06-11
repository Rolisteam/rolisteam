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
#ifndef VECTORIALMAPMESSAGEHELPER_H
#define VECTORIALMAPMESSAGEHELPER_H

#include <QHash>
#include <QString>
#include <QVariant>
#include <core_global.h>
#include <map>

class VectorialMapController;
class NetworkMessageReader;

class CORE_EXPORT VectorialMapMessageHelper
{
public:
    VectorialMapMessageHelper();

    // send off ITEMS
    static void sendOffNewItem(const std::map<QString, QVariant>& args, const QString& mapId);
    static void sendOffHighLight(const QPointF& p, const qreal& penSize, const QColor& color, const QString& mapId);
    static void readHighLight(VectorialMapController* ctrl, NetworkMessageReader* msg);

    // read message items
    static void readVectorialMapController(VectorialMapController* ctrl, const QByteArray& array);
    static QByteArray saveVectorialMap(VectorialMapController* ctrl);
    static void fetchModelFromMap(const QHash<QString, QVariant>& params, VectorialMapController* ctrl);
};

#endif // VECTORIALMAPMESSAGEHELPER_H
