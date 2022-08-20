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
#include "updater/media/charactersheetupdater.h"

#include <QJsonDocument>

#include "charactersheet/charactersheet.h"
#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/csitem.h"
#include "controller/view_controller/charactersheetcontroller.h"
#include "data/character.h"
#include "network/networkmessagewriter.h"
#include "worker/messagehelper.h"

CharacterSheetUpdater::CharacterSheetUpdater(campaign::CampaignManager* campaign, QObject* parent)
    : MediaUpdaterInterface(campaign, parent)
{
    // comment
}

void CharacterSheetUpdater::addMediaController(MediaControllerBase* ctrl)
{
    auto csCtrl= dynamic_cast<CharacterSheetController*>(ctrl);
    if(!csCtrl)
        return;

    m_ctrls.append(csCtrl);

    connect(csCtrl, &CharacterSheetController::share, this, &CharacterSheetUpdater::shareCharacterSheetTo);
    connect(csCtrl, &CharacterSheetController::modifiedChanged, this, []() {

    });
}

void CharacterSheetUpdater::shareCharacterSheetTo(CharacterSheetController* ctrl, CharacterSheet* sheet,
                                                  CharacterSheetUpdater::SharingMode mode, Character* character,
                                                  const QStringList& recipients, bool gmToPlayer)
{
    if(!sheet || (recipients.isEmpty() && mode != SharingMode::ALL))
        return;

    if(localIsGM())
    {
        auto it= std::find_if(std::begin(m_sharingData), std::end(m_sharingData),
                              [sheet](const CSSharingInfo& info) { return sheet->uuid() == info.sheetId; });

        if(it != std::end(m_sharingData))
        {
            auto list= it->recipients;
            for(auto rec : list)
            {
                MessageHelper::stopSharingSheet(it->sheetId, it->ctrlId, rec);
            }
            m_sharingData.erase(it);
        }

        m_sharingData.append(
            {ctrl->uuid(), sheet->uuid(), character ? character->uuid() : QString(), mode, sheet, recipients});
        MessageHelper::shareCharacterSheet(sheet, character, ctrl);
    }

    connect(sheet, &CharacterSheet::updateField, this,
            [recipients, mode, ctrl](CharacterSheet* sheet, CSItem* itemSheet, const QString& path) {
                if(nullptr == sheet)
                    return;

                NetworkMessageWriter msg(NetMsg::CharacterCategory, NetMsg::updateFieldCharacterSheet);
                if(mode != SharingMode::ALL)
                {
                    msg.setRecipientList(recipients, NetworkMessage::OneOrMany);
                }
                msg.string8(ctrl->uuid());
                msg.string8(sheet->uuid());
                msg.string32(path);
                QJsonObject object;
                itemSheet->saveDataItem(object);
                QJsonDocument doc;
                doc.setObject(object);
                msg.byteArray32(doc.toJson());
                msg.sendToServer();
            });
}
