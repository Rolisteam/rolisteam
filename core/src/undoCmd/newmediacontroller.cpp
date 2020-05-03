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

#include <QDebug>
#include <QUuid>

#include "controller/contentcontroller.h"
#include "controller/media_controller/mediamanagerbase.h"

NewMediaController::NewMediaController(Core::ContentType contentType, MediaManagerBase* ctrl,
                                       ContentController* contentCtrl, const std::map<QString, QVariant>& map,
                                       QUndoCommand* parent)
    : QUndoCommand(parent), m_ctrl(ctrl), m_contentType(contentType), m_contentCtrl(contentCtrl), m_args(map)
{
    m_title= m_args["title"].toString();

    QString media(QObject::tr("Create new %1 %2"));

    setText(media.arg(CleverURI::typeToString(contentType)).arg(m_title));
}

void NewMediaController::redo()
{
    qInfo() << QStringLiteral("Redo command newmediacontroller: %1 ").arg(text());
    if(m_ctrl.isNull() || m_contentCtrl.isNull())
        return;

    // m_uri= new CleverURI(m_title, "", "", m_contentType);

    if(m_uuidUri.isEmpty())
        m_uuidUri= QUuid::createUuid().toString(QUuid::WithoutBraces);

    m_ctrl->openMedia(m_uuidUri, m_args);
    // m_contentCtrl->addContent(m_uri); // resources manager
}

void NewMediaController::undo()
{
    qInfo() << QStringLiteral("Undo command newmediacontroller: %1 ").arg(text());
    if(/*nullptr == m_uri ||*/ nullptr == m_ctrl)
        return;

    m_ctrl->closeMedia(m_uuidUri);
    // m_contentCtrl->removeContent(m_uri);

    // m_uri= nullptr;
}
