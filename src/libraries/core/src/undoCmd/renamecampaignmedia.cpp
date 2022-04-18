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
#include "undoCmd/renamecampaignmedia.h"
#include "data/campaign.h"
#include "utils/iohelper.h"
#include "model/mediamodel.h"

#include <QDebug>

RenameCampaignMedia::RenameCampaignMedia(campaign::MediaNode* mediaNode, const QString& newPath, const QString& oldPath,
                                         campaign::MediaModel* model, campaign::Campaign* campaign)
    : QUndoCommand(), m_model(model), m_media(mediaNode), m_newPath(newPath), m_oldPath(oldPath), m_campaign(campaign)
{
}
void RenameCampaignMedia::redo()
{
    if(!m_media)
        return;

    qDebug() << "move " << m_oldPath << m_newPath;

    if(utils::IOHelper::moveFile(m_oldPath, m_newPath))
    {
        m_media->setPath(m_newPath);
        m_model->dataChangedFor(m_media);
        m_campaign->renameMedia(m_media->uuid(), m_newPath);
    }
}
void RenameCampaignMedia::undo()
{
    if(!m_media)
        return;

    qDebug() << "moveundo " << m_newPath << m_oldPath;

    if(utils::IOHelper::moveFile(m_newPath, m_oldPath))
    {
        m_media->setPath(m_oldPath);
        m_model->dataChangedFor(m_media);
        m_campaign->renameMedia(m_media->uuid(), m_oldPath);
    }
}
