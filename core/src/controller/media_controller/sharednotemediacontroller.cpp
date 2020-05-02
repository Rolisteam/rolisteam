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

#include <QSet>
#include <utility>

#include "controller/view_controller/sharednotecontroller.h"
#include "updater/sharednotecontrollerupdater.h"
#include "userlist/playermodel.h"
#include "worker/messagehelper.h"

SharedNoteController* findNote(const std::vector<std::unique_ptr<SharedNoteController>>& notes, const QString& id)
{
    auto it= std::find_if(notes.begin(), notes.end(),
                          [id](const std::unique_ptr<SharedNoteController>& ctrl) { return ctrl->uuid() == id; });
    if(notes.end() == it)
        return nullptr;
    return (*it).get();
}

SharedNoteMediaController::SharedNoteMediaController(PlayerModel* model, QObject* parent)
    : MediaControllerInterface(parent), m_updater(new SharedNoteControllerUpdater), m_playerModel(model)
{
    connect(this, &SharedNoteMediaController::localIdChanged, this, [this](const QString& id) {
        std::for_each(m_sharedNotes.begin(), m_sharedNotes.end(),
                      [id](const std::unique_ptr<SharedNoteController>& ctrl) { ctrl->setOwnerId(id); });
    });
}

SharedNoteMediaController::~SharedNoteMediaController()= default;

CleverURI::ContentType SharedNoteMediaController::type() const
{
    return CleverURI::SHAREDNOTE;
}

bool SharedNoteMediaController::openMedia(CleverURI* uri, const std::map<QString, QVariant>& args)
{
    if(uri == nullptr || (args.empty() && uri->getUri().isEmpty()))
        return false;

    std::unique_ptr<SharedNoteController> notesCtrl(new SharedNoteController(localId(), localId(), m_playerModel, uri));
    // not remote
    notesCtrl->setLocalGM(localIsGM());

    m_updater->addSharedNoteController(notesCtrl.get());
    emit sharedNoteControllerCreated(notesCtrl.get());
    m_sharedNotes.push_back(std::move(notesCtrl));
    return true;
}

void SharedNoteMediaController::addSharedNotes(CleverURI* uri, const QHash<QString, QVariant>& params)
{
    if(uri == nullptr)
        return;

    QString ownerId;
    if(params.contains(QStringLiteral("ownerId")))
    {
        ownerId= params.value(QStringLiteral("ownerId")).toString();
    }

    std::unique_ptr<SharedNoteController> notesCtrl(new SharedNoteController(ownerId, localId(), m_playerModel, uri));
    if(params.contains(QStringLiteral("id")))
    {
        notesCtrl->setUuid(params.value(QStringLiteral("id")).toString());
    }
    if(params.contains(QStringLiteral("markdown")))
    {
        auto b= params.value(QStringLiteral("markdown")).toBool();
        notesCtrl->setHighligthedSyntax(b ? SharedNoteController::HighlightedSyntax::MarkDown :
                                            SharedNoteController::HighlightedSyntax::None);
    }
    if(params.contains(QStringLiteral("text")))
    {
        notesCtrl->setText(params.value(QStringLiteral("text")).toString());
    }

    notesCtrl->setLocalGM(localIsGM());
    notesCtrl->setLocalId(localId());

    m_updater->addSharedNoteController(notesCtrl.get());

    emit sharedNoteControllerCreated(notesCtrl.get());
    m_sharedNotes.push_back(std::move(notesCtrl));
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
        addSharedNotes(new CleverURI(CleverURI::SHAREDNOTE), hash);
    }
    else if(msg->action() == NetMsg::CloseMedia && msg->category() == NetMsg::MediaCategory)
    {
        auto noteId= msg->string8();
        closeMedia(noteId);
    }
    else if(msg->action() == NetMsg::UpdateMediaProperty && msg->category() == NetMsg::MediaCategory)
    {
        auto noteId= msg->string8();
        auto note= findNote(m_sharedNotes, noteId);
        m_updater->updateProperty(msg, note);
    }
    return type;
}

void SharedNoteMediaController::setUndoStack(QUndoStack* stack)
{
    // return;
}
