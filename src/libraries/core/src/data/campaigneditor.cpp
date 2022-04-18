/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#include "data/campaigneditor.h"

#include "data/campaign.h"
#include "data/character.h"
#include "data/media.h"
#include "data/rolisteamtheme.h"
#include "utils/iohelper.h"
#include "model/characterstatemodel.h"
#include "model/nonplayablecharactermodel.h"
#include "worker/fileserializer.h"
#include "worker/iohelper.h"
#include "worker/modelhelper.h"
#include "worker/utilshelper.h"

#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>

namespace campaign
{
namespace
{
void readCampaignInfo(const CampaignInfo& info, Campaign* manager)
{
    if(!info.status || !manager)
        return;
    ModelHelper::fetchDiceModel(info.dices, manager->diceAliases());
    ModelHelper::fetchCharacterStateModel(info.states, manager->stateModel());
    ModelHelper::fetchNpcModel(info.npcs, manager->npcModel(), manager->rootDirectory());
    auto assets= info.asset;
    ModelHelper::fetchMedia(assets[Core::JsonKey::JSON_MEDIAS].toArray(), manager);
    manager->setName(assets[Core::JsonKey::JSON_NAME].toString());
    manager->setCurrentChapter(assets[Core::JsonKey::JSON_CURRENT_CHAPTER].toString());
    manager->setCurrentTheme(IOHelper::jsonToTheme(info.theme));

    auto const& states= manager->stateModel()->statesList();
    auto list= new QList<CharacterState*>();
    std::transform(std::begin(states), std::end(states), std::back_inserter(*list),
                   [](const std::unique_ptr<CharacterState>& item) { return item.get(); });
    Character::setListOfCharacterState(list);
}
} // namespace
CampaignEditor::CampaignEditor(QObject* parent) : QObject(parent), m_campaign(new Campaign)
{
    connect(m_campaign.get(), &Campaign::mediaAdded, this, &CampaignEditor::importedFile);
}

Campaign* CampaignEditor::campaign() const
{
    return m_campaign.get();
}

bool CampaignEditor::open(const QString& from)
{
    m_campaign->setRootDirectory(from);
    helper::utils::setContinuation<CampaignInfo>(
        QtConcurrent::run([from]() { return campaign::FileSerializer::readCampaignDirectory(from); }), m_campaign.get(),
        [this](const CampaignInfo& info)
        {
            readCampaignInfo(info, m_campaign.get());
            emit campaignLoaded(info.missingFiles, info.unmanagedFiles);
        });
    return true;
}

bool CampaignEditor::mergeAudioFile(const QString& source, const QString& dest)
{
    return IOHelper::mergePlayList(source, dest);
}

/*bool CampaignEditor::copyArrayModelAndFile(const QString& source, const QString& srcDir, const QString& dest,
                                           const QString& destDir)
{
    return IOHelper::copyArrayModelAndFile(source, srcDir, dest, destDir);
}*/

bool CampaignEditor::copyMedia(const QString& source, const QString& dest, Core::MediaType type)
{
    auto listFile= IOHelper::mediaList(source, type);
    if(listFile.isEmpty())
        return false;
    for(auto const& file : listFile)
    {
        addMedia(QUuid::createUuid().toString(QUuid::WithoutBraces), QString("%1/%2").arg(dest, file),
                 utils::IOHelper::loadFile(QString("%1/%2").arg(source, file)));
    }
    return true;
}

bool CampaignEditor::copyTheme(const QString& source, const QString& dest)
{
    auto res= utils::IOHelper::copyFile(source, dest);
    return !res.isEmpty();
}

bool CampaignEditor::mergeJsonArrayFile(const QString& source, const QString& dest)
{
    bool ok;
    auto srcArray= IOHelper::loadJsonFileIntoArray(source, ok);
    if(!ok)
        return false;
    auto destArray= IOHelper::loadJsonFileIntoArray(dest, ok);

    if(!ok)
    {
        utils::IOHelper::copyFile(source, dest);
        return true;
    }

    destArray.append(srcArray);
    IOHelper::writeJsonArrayIntoFile(dest, destArray);
    return true;
}

bool CampaignEditor::loadDiceAlias(const QString& source)
{
    if(!m_campaign)
        return false;

    bool ok;
    ModelHelper::fetchDiceModel(IOHelper::loadJsonFileIntoArray(source, ok), m_campaign->diceAliases());
    return ok;
}

bool CampaignEditor::loadStates(const QString& source, const QString& srcDir, const QString& dest,
                                const QString& destDir)
{
    if(!m_campaign)
        return false;

    bool ok;
    ModelHelper::fetchCharacterStateModel(IOHelper::loadJsonFileIntoArray(source, ok), m_campaign->stateModel());
    auto const& list= m_campaign->stateModel()->statesList();

    std::for_each(std::begin(list), std::end(list),
                  [srcDir, destDir](const std::unique_ptr<CharacterState>& state)
                  {
                      auto path= state->imagePath();
                      if(path.isEmpty())
                          return;
                      utils::IOHelper::copyFile(QString("%1/%2").arg(srcDir, path),
                                                QString("%1/%2").arg(destDir, path));
                  });
    // todo manage images from states
    return ok;
}

bool CampaignEditor::loadNpcData(const QString& source, const QString& srcDir, const QString& dest,
                                 const QString& destDir)
{
    if(!m_campaign)
        return false;

    bool ok;
    ModelHelper::fetchNpcModel(IOHelper::loadJsonFileIntoArray(source, ok), m_campaign->npcModel(), srcDir);
    auto const& list= m_campaign->npcModel()->npcList();

    std::for_each(std::begin(list), std::end(list),
                  [srcDir, destDir](const std::unique_ptr<NonPlayableCharacter>& npc)
                  {
                      auto path= npc->avatarPath();
                      if(path.isEmpty())
                          return;
                      utils::IOHelper::copyFile(QString("%1/%2").arg(srcDir, path),
                                                QString("%1/%2").arg(destDir, path));
                  });
    // todo manage images from states
    return ok;
}

bool CampaignEditor::removeMedia(const QString& src)
{
    // remove from campaign
    auto media= m_campaign->mediaFromPath(src);

    if(!media)
        return false;

    m_campaign->removeMedia(media->id());

    // remove file on disk
    return removeFile(src);
}

bool CampaignEditor::removeFile(const QString& src)
{
    return utils::IOHelper::removeFile(src);
}

QString CampaignEditor::saveAvatar(const QString& id, const QByteArray& array)
{
    auto dest= QString("%1/avatar_%2.png").arg(m_campaign->directory(campaign::Campaign::Place::NPC_ROOT), id);
    utils::IOHelper::writeFile(dest, array);
    return dest;
}

/*QString CampaignEditor::addFileIntoCharacters(const QString& src)
{
    // TODO create command
    // IOHelper::copyFile();
    // IOHelper::writeFile(src, array, true);
}

bool CampaignEditor::removeFileFromCharacters(const QString& path)
{
    // IOHelper::removeFile(path);
}*/

bool CampaignEditor::addMedia(const QString& id, const QString& dest, const QByteArray& array)
{
    QFileInfo file(dest);
    std::unique_ptr<Media> media(new Media(id, file.baseName(), dest, FileSerializer::typeFromExtention(dest)));
    m_campaign->addMedia(std::move(media));
    utils::IOHelper::writeFile(dest, array, true);
    return true;
}

QString CampaignEditor::mediaFullPathWithExtension(const QString& file, Core::ContentType type) const
{
    return mediaFullPath(campaign::FileSerializer::addExtention(file, type));
}

QString CampaignEditor::mediaFullPath(const QString& file) const
{
    return QString("%1/%2").arg(m_campaign->directory(campaign::Campaign::Place::MEDIA_ROOT), file);
}

void CampaignEditor::doCommand(QUndoCommand* command)
{
    emit performCommand(command);
}

QString campaign::CampaignEditor::campaignDir() const
{
    return m_campaign->rootDirectory();
}

QString CampaignEditor::currentDir() const
{
    return m_campaign->currentChapter();
}
} // namespace campaign
