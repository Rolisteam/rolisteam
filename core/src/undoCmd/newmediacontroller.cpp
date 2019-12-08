/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#include "newmediacontroller.h"

#include "controller/contentcontroller.h"
#include "controller/media_controller/mediacontrollerinterface.h"

#include <QDebug>

NewMediaController::NewMediaController(CleverURI::ContentType contentType, MediaControllerInterface* ctrl,
                                       ContentController* contentCtrl, bool isGm,
                                       const std::map<QString, QVariant>& map, QUndoCommand* parent)
    : QUndoCommand(parent), m_ctrl(ctrl), m_contentCtrl(contentCtrl), m_args(map), m_gm(isGm)
{
    m_uri= new CleverURI(m_args["title"].toString(), "", "", contentType);

    setText(QObject::tr("Create new media %1").arg(m_args["title"].toString()));
}

void NewMediaController::redo()
{
    qInfo() << QStringLiteral("Redo command newmediacontroller: %1 ").arg(text());
    if(nullptr == m_uri || m_ctrl.isNull() || m_contentCtrl.isNull())
        return;

    m_ctrl->openMedia(m_uri, m_args);
    m_contentCtrl->addContent(m_uri);
}

void NewMediaController::undo()
{
    qInfo() << QStringLiteral("Undo command newmediacontroller: %1 ").arg(text());
    if(nullptr == m_uri || nullptr == m_ctrl)
        return;

    m_ctrl->closeMedia(m_uri->uuid());
    m_contentCtrl->removeContent(m_uri);
}
