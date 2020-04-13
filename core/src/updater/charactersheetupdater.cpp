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
#include "charactersheetupdater.h"

#include <QJsonDocument>

#include "charactersheet/charactersheet.h"
#include "charactersheet/charactersheetmodel.h"

CharacterSheetUpdater::CharacterSheetUpdater(const QString& id, QObject* parent) : QObject(parent), m_mediaId(id)
{
    // comment
}

void CharacterSheetUpdater::setMediaId(const QString& id)
{
    m_mediaId= id;
}

void CharacterSheetUpdater::addCharacterSheetUpdate(CharacterSheet* sheet, CharacterSheetUpdater::UpdateMode mode,
                                                    const QStringList& list)
{
    if(sheet == nullptr)
        return;

    connect(sheet, &CharacterSheet::updateField, this,
            [list, mode, this](CharacterSheet* sheet, CharacterSheetItem* itemSheet, const QString& path) {
                if(nullptr == sheet || !m_updating)
                    return;

                NetworkMessageWriter msg(NetMsg::CharacterCategory, NetMsg::updateFieldCharacterSheet);
                if(mode != UpdateMode::ALL)
                {
                    msg.setRecipientList(list, NetworkMessage::OneOrMany);
                }
                msg.string8(m_mediaId);
                msg.string8(sheet->getUuid());
                msg.string32(path);
                QJsonObject object;
                itemSheet->saveDataItem(object);
                QJsonDocument doc;
                doc.setObject(object);
                msg.byteArray32(doc.toBinaryData());
                msg.sendToServer();
            });
}

void CharacterSheetUpdater::readUpdateMessage(NetworkMessageReader* reader, CharacterSheet* sheet) {}

bool CharacterSheetUpdater::updating() const
{
    return m_updating;
}

void CharacterSheetUpdater::setUpdating(bool b)
{
    if(m_updating == b)
        return;
    m_updating= b;
    emit updatingChanged(m_updating);
}
