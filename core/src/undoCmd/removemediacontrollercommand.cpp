/***************************************************************************
 *	Copyright (C) 2017 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include "removemediacontrollercommand.h"

#include <QDebug>

#include "controller/contentcontroller.h"
#include "controller/media_controller/mediamanagerbase.h"
#include "controller/view_controller/mediacontrollerbase.h"
#include "media/mediafactory.h"
#include "model/contentmodel.h"
#include "worker/iohelper.h"

RemoveMediaControllerCommand::RemoveMediaControllerCommand(MediaControllerBase* ctrl, ContentModel* model,
                                                           QUndoCommand* parent)
    : QUndoCommand(parent), m_ctrl(ctrl), m_model(model)
{
    if(m_ctrl)
    {
        m_uuid= m_ctrl->uuid();
        m_title= m_ctrl->title();
        m_contentType= ctrl->contentType();
        setText(QObject::tr("Close %1").arg(m_ctrl->name()));
        m_data= IOHelper::saveController(m_ctrl);
    }
}

RemoveMediaControllerCommand::~RemoveMediaControllerCommand()= default;

void RemoveMediaControllerCommand::redo()
{
    qInfo() << QStringLiteral("redo command RemoveMediaControllerCommand: %1 ").arg(text());
    if(nullptr == m_ctrl)
        return;

    m_model->removeMedia(m_ctrl->uuid());
}

void RemoveMediaControllerCommand::undo()
{
    qInfo() << QStringLiteral("undo command RemoveMediaControllerCommand: %1 ").arg(text());
    if(nullptr == m_ctrl)
        return;

    auto media= Media::MediaFactory::createLocalMedia(m_uuid, m_contentType, {{"serializedData", m_data}});
    m_model->appendMedia(media);
}
