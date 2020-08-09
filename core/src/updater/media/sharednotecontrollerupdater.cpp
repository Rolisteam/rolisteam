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
#include "data/cleveruri.h"
#include "model/contentmodel.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "worker/convertionhelper.h"
#include "worker/messagehelper.h"

SharedNoteControllerUpdater::SharedNoteControllerUpdater(FilteredContentModel* model, QObject* parent)
    : MediaUpdaterInterface(parent), m_notesModel(model)
{
}

void SharedNoteControllerUpdater::addMediaController(MediaControllerBase* ctrl)
{
    addSharedNoteController(dynamic_cast<SharedNoteController*>(ctrl));
}

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

    connect(noteCtrl, &SharedNoteController::textUpdateChanged, this,
            [this, noteCtrl]() { sendOffChanges<QString>(noteCtrl, QStringLiteral("updateCmd")); });
    connect(noteCtrl, &SharedNoteController::userCanWrite, this,
            [this, noteCtrl](QString id, bool write) { sendOffPermissionChanged(noteCtrl, write, id); });
}

void SharedNoteControllerUpdater::sendOffPermissionChanged(SharedNoteController* ctrl, bool b, const QString& id)
{
    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::UpdateMediaProperty);
    if(!id.isEmpty())
    {
        msg.setRecipientList({id}, NetworkMessage::OneOrMany);
    }
    msg.uint8(static_cast<int>(Core::ContentType::SHAREDNOTE));
    msg.string8(ctrl->uuid());
    msg.string16(QStringLiteral("permission"));
    auto perm= b ? SharedNoteController::Permission::READWRITE : SharedNoteController::Permission::READ;
    Helper::variantToType<SharedNoteController::Permission>(perm, msg);
    msg.sendToServer();
}

void SharedNoteControllerUpdater::updateProperty(NetworkMessageReader* msg, SharedNoteController* ctrl)
{
    if(nullptr == msg || nullptr == ctrl)
        return;

    auto property= msg->string16();

    QVariant var;

    if(property == QStringLiteral("updateCmd"))
    {
        var= QVariant::fromValue(msg->string32());
    }
    else if(property == QStringLiteral("permission"))
    {
        var= QVariant::fromValue(static_cast<SharedNoteController::Permission>(msg->uint8()));
    }

    m_updatingFromNetwork= true;
    auto feedback= ctrl->setProperty(property.toLocal8Bit().data(), var);
    Q_ASSERT(feedback);
    m_updatingFromNetwork= false;
}
