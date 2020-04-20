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
#include "sharednotemediacontroller.h"

#include "controller/view_controller/sharednotecontroller.h"
#include "userlist/playermodel.h"

#include "worker/messagehelper.h"

SharedNoteMediaController::SharedNoteMediaController(PlayerModel* model, QObject* parent)
    : MediaControllerInterface(parent), m_playerModel(model)
{
}

SharedNoteMediaController::~SharedNoteMediaController()= default;

CleverURI::ContentType SharedNoteMediaController::type() const
{
    return CleverURI::WEBVIEW;
}

bool SharedNoteMediaController::openMedia(CleverURI* uri, const std::map<QString, QVariant>& args)
{
    if(uri == nullptr || (args.empty() && uri->getUri().isEmpty()))
        return false;

    std::unique_ptr<SharedNoteController> webCtrl(new SharedNoteController(m_playerModel, uri));

    emit sharedNoteControllerCreated(webCtrl.get());
    m_sharedNotes.push_back(std::move(webCtrl));
    return true;
}

void SharedNoteMediaController::closeMedia(const QString& id)
{
    auto it= std::remove_if(m_sharedNotes.begin(), m_sharedNotes.end(),
                            [id](const std::unique_ptr<SharedNoteController>& ctrl) { return ctrl->uuid() == id; });
    if(it == m_sharedNotes.end())
        return;

    (*it)->aboutToClose();
    m_sharedNotes.erase(it, m_sharedNotes.end());
}

void SharedNoteMediaController::registerNetworkReceiver()
{
    // ReceiveEvent::registerNetworkReceiver(NetMsg::VMapCategory, this);
}

NetWorkReceiver::SendType SharedNoteMediaController::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type= NetWorkReceiver::NONE;
    if(msg->action() == NetMsg::AddMedia && msg->category() == NetMsg::MediaCategory)
    {
        auto hash= MessageHelper::readSharedNoteData(msg);
    }
    return type;
}

void SharedNoteMediaController::setUndoStack(QUndoStack* stack)
{
    // return;
}
