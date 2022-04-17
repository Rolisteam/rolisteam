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
#ifndef MEDIAFACTORY_H
#define MEDIAFACTORY_H

#include "controller/view_controller/mediacontrollerbase.h"
#include <core_global.h>
class NetworkMessageReader;
namespace Media
{
class CORE_EXPORT MediaFactory
{
public:
    static MediaControllerBase* createLocalMedia(const QString& uuid, Core::ContentType type,
                                                 const std::map<QString, QVariant>& map, bool localIsGM);
    static MediaControllerBase* createRemoteMedia(Core::ContentType type, NetworkMessageReader* msg, bool localIsGM);

    static void setLocalId(const QString& id);

private:
    static QString m_localId;
};
} // namespace Media

#endif // MEDIAFACTORY_H
