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
#include "sharednotecontroller.h"

#include "data/player.h"
#include "sharededitor/participantmodel.h"
#include "userlist/playermodel.h"
#include "worker/iohelper.h"

QPointer<PlayerModel> SharedNoteController::m_playerModel;

SharedNoteController::SharedNoteController(const QString& ownerId, const QString& local, const QString& uuid,
                                           QObject* parent)
    : MediaControllerBase(uuid, Core::ContentType::SHAREDNOTE, parent)
    , m_participantModel(new ParticipantModel(ownerId, m_playerModel))
{
    setOwnerId(ownerId);
    setLocalId(local);

    connect(this, &SharedNoteController::ownerIdChanged, m_participantModel.get(), &ParticipantModel::setOwner);
    connect(this, &SharedNoteController::localIdChanged, this, [this]() {

    });

    connect(this, &SharedNoteController::pathChanged, this,
            [this](const QString& path) { setText(IOHelper::readTextFile(path)); });

    connect(m_participantModel.get(), &ParticipantModel::userReadPermissionChanged, this,
            [this](const QString& id, bool b) {
                if(b)
                    emit openShareNoteTo(id);
                else
                    emit closeShareNoteTo(id);
            });
    connect(m_participantModel.get(), &ParticipantModel::userWritePermissionChanged, this,
            &SharedNoteController::userCanWrite);

    if(localIsOwner())
        setPermission(Permission::READWRITE);

    setParticipantPanelVisible(localIsOwner());
}

SharedNoteController::~SharedNoteController()= default;

void SharedNoteController::setPlayerModel(PlayerModel* model)
{
    m_playerModel= model;
}

QString SharedNoteController::text() const
{
    return m_text;
}

bool SharedNoteController::localCanWrite() const
{
    return (m_permission == SharedNoteController::Permission::READWRITE);
}

QString SharedNoteController::textUpdate() const
{
    return m_latestCommand;
}

SharedNoteController::Permission SharedNoteController::permission() const
{
    return m_permission;
}

bool SharedNoteController::participantPanelVisible() const
{
    return m_participantVisible;
}

ParticipantModel* SharedNoteController::participantModel() const
{
    return m_participantModel.get();
}

SharedNoteController::HighlightedSyntax SharedNoteController::highligthedSyntax() const
{
    return m_highlightedSyntax;
}

bool SharedNoteController::markdownVisible() const
{
    return m_markdownPreview;
}

PlayerModel* SharedNoteController::playerModel() const
{
    return m_playerModel;
}

bool SharedNoteController::canWrite(Player* player) const
{
    if(nullptr == player)
        return false;
    return (m_participantModel->permissionFor(player->uuid()) == ParticipantModel::readWrite);
}

bool SharedNoteController::canRead(Player* player) const
{
    if(nullptr == player)
        return false;
    bool write= canWrite(player);
    bool read= (m_participantModel->permissionFor(player->uuid()) == ParticipantModel::readOnly);
    return write || read;
}

QString SharedNoteController::updateCmd() const
{
    return m_latestCommand;
}

void SharedNoteController::setParticipantPanelVisible(bool b)
{
    if(b == m_participantVisible)
        return;
    m_participantVisible= b;
    emit participantPanelVisibleChanged(m_participantVisible);
}

void SharedNoteController::setPermission(SharedNoteController::Permission permission)
{
    if(permission == m_permission)
        return;
    m_permission= permission;
    emit permissionChanged(m_permission);
}

void SharedNoteController::setText(const QString& text)
{
    if(text == m_text)
        return;
    m_text= text;
    emit textChanged(m_text);
}

void SharedNoteController::setHighligthedSyntax(SharedNoteController::HighlightedSyntax syntax)
{
    if(m_highlightedSyntax == syntax)
        return;
    m_highlightedSyntax= syntax;
    emit highligthedSyntaxChanged();
}

void SharedNoteController::demoteCurrentItem(const QModelIndex& index)
{
    m_participantModel->demotePlayer(index);
}

void SharedNoteController::promoteCurrentItem(const QModelIndex& index)
{
    m_participantModel->promotePlayer(index);
}

void SharedNoteController::setMarkdownVisible(bool b)
{
    if(m_markdownPreview == b)
        return;
    m_markdownPreview= b;
    emit markdownVisibleChanged(m_markdownPreview);
}

void SharedNoteController::setTextUpdate(const QString& cmd)
{
    if(cmd == m_latestCommand)
        return;
    m_latestCommand= cmd;
    emit textUpdateChanged(m_latestCommand);
}

void SharedNoteController::setUpdateCmd(const QString& cmd)
{
    QRegExp rx;
    auto tmpCmd= cmd;
    if(!cmd.startsWith("doc:"))
        return;

    tmpCmd.remove(0, 4);
    rx= QRegExp("(\\d+)\\s(\\d+)\\s(\\d+)\\s(.*)");
    if(!cmd.contains(rx))
        return;

    int pos= rx.cap(1).toInt();
    int charsRemoved= rx.cap(2).toInt();
    int charsAdded= rx.cap(3).toInt();
    tmpCmd= rx.cap(4);
    emit updateCmdChanged();
    emit collabTextChanged(pos, charsRemoved, charsAdded, tmpCmd);
}
