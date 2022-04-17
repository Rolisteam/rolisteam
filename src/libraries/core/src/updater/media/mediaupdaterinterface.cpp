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
#include "updater/media/mediaupdaterinterface.h"

#include "data/campaign.h"
#include "data/campaignmanager.h"
#include "worker/fileserializer.h"
#include "worker/iohelper.h"

#include <QDebug>

MediaUpdaterInterface::MediaUpdaterInterface(campaign::CampaignManager* campaign, QObject* object)
    : QObject(object), m_manager(campaign)
{
}

NetWorkReceiver::SendType MediaUpdaterInterface::processMessage(NetworkMessageReader*)
{
    return NetWorkReceiver::NONE;
}

void MediaUpdaterInterface::saveMediaController(MediaControllerBase* ctrl)
{
    if(!ctrl)
        return;

    auto id= ctrl->uuid();
    auto campaign= m_manager->campaign();
    auto path= ctrl->url().toLocalFile();
    qDebug() << path << "savemediacontroller";
    if(campaign)
    {
        auto p= campaign->pathFromUuid(id);
        qDebug() << "saveMediaController: " << p << path;
        if(!p.isEmpty())
            path= p;
    }
    ctrl->setUrl(QUrl::fromLocalFile(path));

    campaign::FileSerializer::writeFileIntoCampaign(path, IOHelper::saveController(ctrl));

    ctrl->setModified(false);
}
