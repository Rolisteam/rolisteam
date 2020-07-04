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
#include "undoCmd/removemediacontrollercommand.h"
#include "updater/sharednotecontrollerupdater.h"
#include "userlist/playermodel.h"
#include "worker/iohelper.h"
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
    : MediaManagerBase(Core::ContentType::SHAREDNOTE, parent)
    , m_updater(new SharedNoteControllerUpdater)
    , m_playerModel(model)
{
    connect(this, &SharedNoteMediaController::localIdChanged, this, [this](const QString& id) {
        std::for_each(m_sharedNotes.begin(), m_sharedNotes.end(),
                      [id](const std::unique_ptr<SharedNoteController>& ctrl) { ctrl->setOwnerId(id); });
    });
}

SharedNoteMediaController::~SharedNoteMediaController()= default;

bool SharedNoteMediaController::openMedia(const QString& id, const std::map<QString, QVariant>& args)
{
    if(id.isEmpty() && args.empty())
        return false;

    // read params
    QByteArray serializedData;
    auto it= args.find(QStringLiteral("serializedData"));
    if(it != args.end())
        serializedData= it->second.toByteArray();

    // create controller
    std::unique_ptr<SharedNoteController> notesCtrl(new SharedNoteController(localId(), localId(), m_playerModel, id));
    if(!serializedData.isEmpty())
    {
        IOHelper::readSharedNoteController(notesCtrl.get(), serializedData);
    }
    // not remote
    notesCtrl->setLocalGM(localIsGM());

    auto notes= notesCtrl.get();
    processNewController(notes, true);

    m_sharedNotes.push_back(std::move(notesCtrl));
    return true;
}

void SharedNoteMediaController::processNewController(SharedNoteController* notesCtrl, bool local)
{
    m_updater->addSharedNoteController(notesCtrl);
    emit sharedNoteControllerCreated(notesCtrl);

    if(!local)
        return;

    emit mediaAdded(notesCtrl->uuid(), notesCtrl->path(), notesCtrl->contentType(), notesCtrl->name());
    connect(notesCtrl, &SharedNoteController::closeMe, this, [this, notesCtrl]() {
        if(!m_undoStack)
            return;
        m_undoStack->push(new RemoveMediaControllerCommand(notesCtrl, this));
    });
}

void SharedNoteMediaController::addSharedNotes(const QHash<QString, QVariant>& params)
{
    auto ownerId= params.value(QStringLiteral("ownerId")).toString();
    auto uuid= params.value(QStringLiteral("id")).toString();

    std::unique_ptr<SharedNoteController> notesCtrl(new SharedNoteController(ownerId, localId(), m_playerModel, uuid));

    if(params.contains(QStringLiteral("markdown")))
    {
        auto b= params.value(QStringLiteral("markdown")).toBool();
        notesCtrl->setHighligthedSyntax(b ? SharedNoteController::HighlightedSyntax::MarkDown :
                                            SharedNoteController::HighlightedSyntax::None);
    }

    notesCtrl->setText(params.value(QStringLiteral("text")).toString());

    notesCtrl->setLocalGM(localIsGM());
    notesCtrl->setLocalId(localId());

    processNewController(notesCtrl.get(), false);

    // m_updater->addSharedNoteController(notesCtrl.get());

    // emit sharedNoteControllerCreated(notesCtrl.get());
    m_sharedNotes.push_back(std::move(notesCtrl));
}

std::vector<SharedNoteController*> SharedNoteMediaController::controllers() const
{
    std::vector<SharedNoteController*> vec;
    std::transform(m_sharedNotes.begin(), m_sharedNotes.end(), std::back_inserter(vec),
                   [](const std::unique_ptr<SharedNoteController>& ctrl) { return ctrl.get(); });
    return vec;
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
        addSharedNotes(hash);
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

int SharedNoteMediaController::managerCount() const
{
    return static_cast<int>(m_sharedNotes.size());
}
