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
#include "sharednotecontrollerupdater.h"

#include <QSet>

#include "controller/view_controller/sharednotecontroller.h"
#include "network/networkmessagewriter.h"
#include "worker/messagehelper.h"

SharedNoteControllerUpdater::SharedNoteControllerUpdater(QObject* parent) : QObject(parent) {}

void SharedNoteControllerUpdater::addSharedNoteController(SharedNoteController* noteCtrl)
{
    if(noteCtrl == nullptr || m_noteReaders.find(noteCtrl) != m_noteReaders.end())
        return;

    m_noteReaders.insert({noteCtrl, QSet<QString>()});

    connect(noteCtrl, &SharedNoteController::openShareNoteTo, this, [this, noteCtrl](const QString& id) {
        MessageHelper::shareNotesTo(noteCtrl, {id});
        try
        {
            auto& it= m_noteReaders.at(noteCtrl);
            it.insert(id);
        }
        catch(const std::out_of_range&)
        {
            Q_ASSERT(false);
        }
    });

    connect(noteCtrl, &SharedNoteController::closeShareNoteTo, this, [this, noteCtrl](const QString& id) {
        MessageHelper::closeNoteTo(noteCtrl, id);
        try
        {
            auto& it= m_noteReaders.at(noteCtrl);
            it.remove(id);
        }
        catch(...)
        {
            Q_ASSERT(false);
        }
    });

    connect(noteCtrl, &SharedNoteController::partialChangeOnText, this, [this, noteCtrl](const QString& cmd) {
        QStringList list;
        try
        {
            auto listener= m_noteReaders.at(noteCtrl);
            list << listener.values();
        }
        catch(...)
        {
        }

        NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::CloseMedia);
        msg.setRecipientList(list, NetworkMessage::OneOrMany);
        msg.string8(noteCtrl->uuid());
        msg.string32(cmd);
        msg.sendToServer();
    });
}

void SharedNoteControllerUpdater::readUpdateCommand(NetworkMessageReader* reader, SharedNoteController* ctrl) {}
