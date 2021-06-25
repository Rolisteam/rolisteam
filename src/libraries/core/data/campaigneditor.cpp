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
#include "campaigneditor.h"

#include "campaign.h"
#include "data/media.h"
#include "data/rolisteamtheme.h"
#include "worker/fileserializer.h"
#include "worker/iohelper.h"
#include "worker/modelhelper.h"

#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>

namespace campaign
{
namespace
{
void readCampaignInfo(const CampaignInfo& info, Campaign* manager)
{
    ModelHelper::fetchDiceModel(info.dices, manager->diceAliases());
    ModelHelper::fetchCharacterStateModel(info.states, manager->stateModel());
    ModelHelper::fetchNpcModel(info.npcs, manager->npcModel(), manager->rootDirectory());
    auto assets= info.asset;
    ModelHelper::fetchMedia(assets[Core::JsonKey::JSON_MEDIAS].toArray(), manager);
    manager->setName(assets[Core::JsonKey::JSON_NAME].toString());
    manager->setCurrentChapter(assets[Core::JsonKey::JSON_CURRENT_CHAPTER].toString());
    manager->setCurrentTheme(IOHelper::jsonToTheme(info.theme));

    qDebug() << "unamanaged file:" << info.unmanagedFiles;
    qDebug() << "missingFiles file:" << info.missingFiles;
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

void CampaignEditor::createNew(const QString& dir)
{
    FileSerializer::createCampaignDirectory(dir);
}

bool CampaignEditor::open(const QString& from, bool discard)
{
    // setState(State::Checking);
    QFutureWatcher<CampaignInfo>* watcher= new QFutureWatcher<CampaignInfo>();
    connect(watcher, &QFutureWatcher<CampaignInfo>::finished, watcher, [this, watcher]() {
        auto info= watcher->result();
        readCampaignInfo(info, m_campaign.get());
        emit campaignChanged();
        delete watcher;
    });
    m_campaign->setRootDirectory(from);
    watcher->setFuture(QtConcurrent::run([from]() { return campaign::FileSerializer::readCampaignDirectory(from); }));
    return true;
}

bool CampaignEditor::save(const QString& to) {}

bool CampaignEditor::saveCopy(const QString& src, const QString& to)
{
    // save copy
    // save
    // copy to dest
    // change root
}

bool CampaignEditor::removeMedia(const QString& src)
{
    // remove from campaign
    auto media= m_campaign->mediaFromPath(src);

    if(!media)
        return false;

    m_campaign->removeMedia(media->id());

    // remove file on disk
    IOHelper::removeFile(src);

    return true;
}

QString CampaignEditor::saveAvatar(const QString& id, const QByteArray& array)
{
    auto dest= QString("%1/avatar_%2.png").arg(m_campaign->directory(campaign::Campaign::Place::NPC_ROOT), id);
    IOHelper::writeFile(dest, array);
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

bool CampaignEditor::addMedia(const QString& src, const QByteArray& array)
{
    QFileInfo file(src);
    std::unique_ptr<Media> media(
        new Media(QUuid::createUuid().toString(), file.baseName(), src, FileSerializer::typeFromExtention(src)));
    m_campaign->addMedia(std::move(media));
    IOHelper::writeFile(src, array, true);
    return true;
}

QString CampaignEditor::mediaFullPath(const QString& file, Core::ContentType type)
{
    return QString("%1/%2").arg(m_campaign->directory(campaign::Campaign::Place::MEDIA_ROOT),
                                campaign::FileSerializer::addExtention(file, type));
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
