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
#ifndef GENERICUPDATER_H
#define GENERICUPDATER_H

#include "mediaupdaterinterface.h"
#include <QObject>
#include <core_global.h>
class CORE_EXPORT GenericUpdater : public MediaUpdaterInterface
{
    Q_OBJECT
public:
    GenericUpdater(campaign::CampaignManager* campaign);

    void addMediaController(MediaControllerBase* ctrl) override;

signals:
    void shareMedia(MediaControllerBase* ctrl);
    void stopSharingMedia(MediaControllerBase* ctrl);
};

#endif // GENERICUPDATER_H
