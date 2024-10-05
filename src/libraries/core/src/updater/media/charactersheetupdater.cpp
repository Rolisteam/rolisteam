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
#include <QLoggingCategory>

#include "charactersheet/charactersheet.h"
#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/csitem.h"
#include "common/logcategory.h"
#include "controller/view_controller/charactersheetcontroller.h"
#include "data/character.h"
#include "media/mediafactory.h"
#include "network/networkmessagewriter.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"

namespace
{

CharacterSheetController* findController(const QString& mediaId, const QString& sheetId,
                                         const QList<QPointer<CharacterSheetController>>& ctrls)
{
    auto it= std::find_if(std::begin(ctrls), std::end(ctrls),
                          [mediaId, sheetId](const QPointer<CharacterSheetController>& ctrl)
                          { return ctrl->uuid() == mediaId && ctrl->hasCharacterSheet(sheetId); });

    if(it == std::end(ctrls))
        return nullptr;

    return (*it).get();
}

} // namespace

CharacterSheetUpdater::CharacterSheetUpdater(FilteredContentModel* model, campaign::CampaignManager* campaign,
                                             QObject* parent)
    : MediaUpdaterInterface(campaign, parent), m_model(model), m_finder{new CharacterFinder()}
{
    if(!m_finder->setUpConnect())
        qCWarning(CharacterSheetCat, "Character Finder is not ready.");
    // comment
    ReceiveEvent::registerNetworkReceiver(NetMsg::CharacterSheetCategory, this);
    connect(m_finder.get(), &CharacterFinder::characterAdded, this,
            [this](const QStringList& list)
            {
                std::for_each(std::begin(m_ctrls), std::end(m_ctrls),
                              [list](const QPointer<CharacterSheetController>& ctrl)
                              {
                                  auto sharingData= ctrl->sheetData();

                                  for(auto const& info : sharingData)
                                  {
                                      if(list.contains(info.m_characterId))
                                      {
                                          ctrl->shareCharacterSheetTo(info);
                                      }
                                  }
                              });
            });
}

void CharacterSheetUpdater::addMediaController(MediaControllerBase* ctrl)
{
    auto csCtrl= dynamic_cast<CharacterSheetController*>(ctrl);
    if(!csCtrl)
        return;

    m_ctrls.append(csCtrl);

    connect(csCtrl, &CharacterSheetController::share, this,
            [this](CharacterSheetController* ctrl, CharacterSheet* sheet, CharacterSheetUpdater::SharingMode mode,
                   Character* character, const QStringList& recipients)
            { shareCharacterSheetTo(ctrl, sheet, mode, character, recipients); });

    connect(csCtrl, &CharacterSheetController::modifiedChanged, this,
            [this]()
            {
                qDebug() << "Modified charactersheet controller";
                auto ctrl= qobject_cast<CharacterSheetController*>(sender());
                if(!ctrl)
                    return;
                saveMediaController(ctrl);
            });

    connect(csCtrl, &CharacterSheetController::removedSheet, this,
            [](const QString& uuid, const QString& ctrlId, const QString& characterId)
            { MessageHelper::stopSharingSheet(uuid, ctrlId, characterId); });
}

void CharacterSheetUpdater::addRemoteCharacterSheet(CharacterSheetController* ctrl)
{
    if(!ctrl->remote())
        return;

    auto const& data= ctrl->sheetData();
    qDebug() << "[sheet] Remote connect to all sheets:" << data.size();
    for(auto const& sheetInfo : data)
    {
        auto sheet= ctrl->characterSheetFromId(sheetInfo.m_sheetId);
        if(!sheet)
            continue;
        m_sharingData.append({ctrl->uuid(),
                              sheetInfo.m_sheetId,
                              sheetInfo.m_characterId,
                              CharacterSheetUpdater::SharingMode::ONE,
                              sheet,
                              {ctrl->gameMasterId()}});
        setUpFieldUpdate(sheet);
    }
}

void CharacterSheetUpdater::shareCharacterSheetTo(CharacterSheetController* ctrl, CharacterSheet* sheet,
                                                  CharacterSheetUpdater::SharingMode mode, Character* character,
                                                  const QStringList& recipients)
{
    if(!sheet || (recipients.isEmpty() && mode != SharingMode::ALL))
        return;

    if(localIsGM())
    {
        auto const it= std::find_if(std::begin(m_sharingData), std::end(m_sharingData),
                                    [sheet](const CSSharingInfo& info) { return sheet->uuid() == info.sheetId; });

        if(it != std::end(m_sharingData))
        {
            auto list= it->recipients;
            for(auto& rec : list)
            {
                MessageHelper::stopSharingSheet(it->sheetId, it->ctrlId, rec);
            }
            m_sharingData.erase(it);
        }

        m_sharingData.append(
            {ctrl->uuid(), sheet->uuid(), character ? character->uuid() : QString(), mode, sheet, recipients});
        MessageHelper::shareCharacterSheet(sheet, character, ctrl, mode);
    }

    setUpFieldUpdate(sheet);
}

void CharacterSheetUpdater::setUpFieldUpdate(CharacterSheet* sheet) const
{
    connect(sheet, &CharacterSheet::updateField, this, &CharacterSheetUpdater::updateField);
}

NetWorkReceiver::SendType CharacterSheetUpdater::processMessage(NetworkMessageReader* msg)
{
    if(checkAction(msg, NetMsg::CharacterSheetCategory, NetMsg::addCharacterSheet))
    {
        emit characterSheetAdded(msg);
    }
    else if(checkAction(msg, NetMsg::CharacterSheetCategory, NetMsg::updateFieldCharacterSheet))
    {
        qDebug() << "[sheet] NetMsg::CharacterSheetCategory, NetMsg::updateFieldCharacterSheet";
        auto idMedia= msg->string8();
        auto idSheet= msg->string8();
        auto ctrl= findController(idMedia, idSheet, m_ctrls);
        qDebug() << "[sheet]" << idMedia << idSheet << m_ctrls.size() << ctrl;
        if(ctrl)
        {
            qDebug() << "[sheet] inside if";
            auto path= msg->string32();
            auto array= msg->byteArray32();
            auto sheet= ctrl->characterSheetFromId(idSheet);
            if(sheet)
            {
                qDebug() << "[sheet] inside if sheet";
                sheet->setFieldData(IOHelper::textByteArrayToJsonObj(array), path);
            }
        }
    }
    else if(checkAction(msg, NetMsg::CharacterSheetCategory, NetMsg::closeCharacterSheet))
    {
        auto sheetId= msg->string8();
        auto ctrlId= msg->string8();
        auto characterId= msg->string8();
        emit characterSheetRemoved(sheetId, ctrlId, characterId);
    }

    return NetWorkReceiver::NONE;
}

void CharacterSheetUpdater::updateField(CharacterSheet* sheet, CSItem* itemSheet, const QString& path)
{
    if(nullptr == sheet)
        return;

    qDebug() << "[sheet] NetMsg::CharacterSheetCategory count :" << m_sharingData.size();
    std::for_each(std::begin(m_sharingData), std::end(m_sharingData),
                  [sheet, itemSheet, path](const CSSharingInfo& info)
                  {
                      if(info.sheet != sheet)
                          return;

                      NetworkMessageWriter msg(NetMsg::CharacterSheetCategory, NetMsg::updateFieldCharacterSheet);
                      if(info.mode != SharingMode::ALL)
                      {
                          msg.setRecipientList(info.recipients, NetworkMessage::OneOrMany);
                      }
                      msg.string8(info.ctrlId);
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
