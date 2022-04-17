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
#ifndef HELPER_NETWORK_H
#define HELPER_NETWORK_H

#include <QJsonObject>

#include "network/channelmodel.h"
#include <network_global.h>
class ChannelModel;
namespace helper
{
namespace network
{
NETWORK_EXPORT QJsonObject channelModelToJSonObject(ChannelModel* model);
NETWORK_EXPORT QByteArray jsonObjectToByteArray(const QJsonObject& obj);
NETWORK_EXPORT void fetchChannelModel(ChannelModel* model, const QJsonObject& obj);

} // namespace network
}; // namespace helper

#endif // HELPER_NETWORK_H
