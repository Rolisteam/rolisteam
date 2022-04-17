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
#include "notemediacontroller.h"

#include "undoCmd/removemediacontrollercommand.h"

#include "controller/view_controller/notecontroller.h"
#include "worker/iohelper.h"

NoteMediaController::NoteMediaController(ContentModel* contentModel, QObject* parent)
    : MediaManagerBase(Core::ContentType::NOTES, contentModel, parent)
{
}

NoteMediaController::~NoteMediaController()= default;

/*bool NoteMediaController::openMedia(const QString& uuid, const std::map<QString, QVariant>& args)
{
    std::unique_ptr<NoteController> noteCtrl(new NoteController(uuid));

    if(args.find("name") != args.end())
        noteCtrl->setName(args.at("name").toString());

    if(args.find("path") != args.end())
        noteCtrl->setPath(args.at("path").toString());

    if(args.find("ownerId") != args.end())
        noteCtrl->setOwnerId(args.at("ownerId").toString());

    if(args.find("serializedData") != args.end())
    {
        QByteArray data= args.at("serializedData").toByteArray();
        IOHelper::readNoteController(noteCtrl.get(), data);
    }
    auto note= noteCtrl.get();
    emit noteControllerCreated(noteCtrl.get());
    emit mediaAdded(uuid, noteCtrl->path(), type(), noteCtrl->name());
    connect(noteCtrl.get(), &NoteController::closeMe, this, [this, note]() {
        if(!m_undoStack)
            return;
        m_undoStack->push(new RemoveMediaControllerCommand(note, this));
    });

    m_notes.push_back(std::move(noteCtrl));
    return true;
}

void NoteMediaController::closeMedia(const QString& id)
{
    auto it= std::remove_if(m_notes.begin(), m_notes.end(),
                            [id](const std::unique_ptr<NoteController>& ctrl) { return ctrl->uuid() == id; });
    if(it == m_notes.end())
        return;

    (*it)->aboutToClose();
    emit mediaClosed(id);
    m_notes.erase(it, m_notes.end());
}*/

void NoteMediaController::registerNetworkReceiver() {}

NetWorkReceiver::SendType NoteMediaController::processMessage(NetworkMessageReader* msg)
{
    return NetWorkReceiver::NONE;
}

int NoteMediaController::managerCount() const
{
    return static_cast<int>(m_notes.size());
}

std::vector<NoteController*> NoteMediaController::controllers() const
{
    std::vector<NoteController*> vec;
    std::transform(m_notes.begin(), m_notes.end(), std::back_inserter(vec),
                   [](const std::unique_ptr<NoteController>& ctrl) { return ctrl.get(); });
    return vec;
}
