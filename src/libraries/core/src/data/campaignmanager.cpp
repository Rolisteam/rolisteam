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
#include "data/campaignmanager.h"

#include "data/media.h"
#include "model/characterstatemodel.h"
#include "model/dicealiasmodel.h"
#include "undoCmd/importdatamedia.h"
#include "undoCmd/importmedia.h"
#include "worker/fileserializer.h"
#include "worker/modelhelper.h"

#include "data/campaigneditor.h"
#include "updater/media/campaignupdater.h"

#include <QDir>
#include <QLoggingCategory>
#include <QUrl>

namespace campaign
{
Q_LOGGING_CATEGORY(CampaignCat, "campaign")
CampaignManager::CampaignManager(DiceRoller* diceparser, QObject* parent)
    : QObject(parent)
    , m_editor(new CampaignEditor)
    , m_campaignUpdater(new CampaignUpdater(diceparser, m_editor->campaign()))
{
    connect(m_editor.get(), &CampaignEditor::importedFile, this, &CampaignManager::fileImported);
    connect(m_editor.get(), &CampaignEditor::campaignLoaded, this, &CampaignManager::campaignLoaded);
    connect(this, &CampaignManager::campaignLoaded, m_campaignUpdater.get(), &CampaignUpdater::updateDiceAliases);
}

CampaignManager::~CampaignManager()= default;

bool CampaignManager::createCampaign(const QUrl& dirUrl)
{
    auto dirPath= dirUrl.toLocalFile();
    if(!m_campaignUpdater->createCampaignTemplate(dirPath))
    {
        emit errorOccured(tr("Impossible to create campaign template at %1").arg(dirPath));
        return false;
    }

    m_editor.reset(new CampaignEditor());
    connect(m_editor.get(), &CampaignEditor::importedFile, this, &CampaignManager::fileImported);
    connect(m_editor.get(), &CampaignEditor::campaignLoaded, this, &CampaignManager::campaignLoaded);
    m_editor->open(dirPath);
    m_campaignUpdater->setCampaign(m_editor->campaign());
    emit campaignChanged(dirPath);
    return true;
}

QString CampaignManager::importFile(const QUrl& urlPath)
{
    if(!m_editor)
        return {};

    auto path= urlPath.toLocalFile();
    auto const name= QFileInfo(path).baseName();
    auto campaign= m_editor->campaign();

    auto destPath= QString("%1/%2").arg(campaign->directory(Campaign::Place::MEDIA_ROOT), m_editor->currentDir());

    auto cmd= new commands::ImportMedia(m_editor->campaign(), name, path, destPath);
    auto res= cmd->destination();
    m_editor->doCommand(cmd);
    return res;
}

QString CampaignManager::createFileFromData(const QString& name, const QByteArray& data)
{
    if(!m_editor)
        return {};

    auto campaign= m_editor->campaign();

    auto cmd= new commands::ImportDataMedia(m_editor->campaign(), name, data,
                                            campaign->directory(Campaign::Place::MEDIA_ROOT));
    auto res= cmd->destination();
    m_editor->doCommand(cmd);

    return res;
}

void CampaignManager::removeFile(const QString& file)
{
    if(m_editor)
        m_editor->removeMedia(file);
}

void CampaignManager::saveCampaign()
{
    if(!m_editor)
        return;
    m_campaignUpdater->save();
}

void CampaignManager::reload()
{
    if(m_editor)
        openCampaign(m_editor->campaignDir());
}

void CampaignManager::copyCampaign(const QUrl& dir)
{
    if(!m_editor)
        return;
    m_campaignUpdater->saveCampaignTo(dir.toLocalFile());
}

void CampaignManager::openCampaign(const QUrl& dir)
{
    auto path= dir.toLocalFile();
    if(!m_campaignUpdater->createCampaignTemplate(path))
    {
        emit errorOccured(tr("Campaign Template creation failed on this location: %1").arg(path));
    }
    m_editor->open(path);
    emit campaignChanged(path);
}

Campaign* CampaignManager::campaign() const
{
    return m_editor->campaign();
}

CampaignEditor* CampaignManager::editor() const
{
    return m_editor.get();
}

DiceRoller* CampaignManager::diceparser() const
{
    return m_campaignUpdater->diceParser();
}

QString CampaignManager::placeDirectory(campaign::Campaign::Place place) const
{
    if(!m_editor || !m_editor->campaign())
        return {};

    return m_editor->campaign()->directory(place);
}

QString CampaignManager::campaignDir() const
{
    return m_editor->campaignDir();
}
void CampaignManager::shareModels()
{
    m_campaignUpdater->updateDiceModel();
    m_campaignUpdater->updateStateModel();
}

void CampaignManager::setLocalIsGM(bool b)
{
    m_campaignUpdater->setLocalIsGM(b);
}

void CampaignManager::importDataFrom(const QString& source, const QVector<Core::CampaignDataCategory>& categories)
{
    if(!m_editor)
        return;

    auto makeSource= [source](const QString& file) { return QString("%1/%2").arg(source, file); };

    for(auto const& cat : categories)
    {
        switch(cat)
        {
        case Core::CampaignDataCategory::AudioPlayer1:
            m_editor->mergeAudioFile(makeSource(FIRST_AUDIO_PLAYER_FILE),
                                     placeDirectory(Campaign::Place::FIRST_AUDIO_PLAYER_FILE));
            break;
        case Core::CampaignDataCategory::AudioPlayer2:
            m_editor->mergeAudioFile(makeSource(SECOND_AUDIO_PLAYER_FILE),
                                     placeDirectory(Campaign::Place::SECOND_AUDIO_PLAYER_FILE));
            break;
        case Core::CampaignDataCategory::AudioPlayer3:
            m_editor->mergeAudioFile(makeSource(THIRD_AUDIO_PLAYER_FILE),
                                     placeDirectory(Campaign::Place::THIRD_AUDIO_PLAYER_FILE));
            break;
        case Core::CampaignDataCategory::AntagonistList:
            m_editor->loadNpcData(makeSource(CHARACTER_MODEL), makeSource(CHARACTER_ROOT),
                                  placeDirectory(Campaign::Place::NPC_MODEL),
                                  placeDirectory(Campaign::Place::NPC_ROOT));
            break;
        case Core::CampaignDataCategory::Images:
            m_editor->copyMedia(makeSource(MEDIA_ROOT), placeDirectory(Campaign::Place::MEDIA_ROOT),
                                Core::MediaType::ImageFile);
            break;
        case Core::CampaignDataCategory::Maps:
            m_editor->copyMedia(makeSource(MEDIA_ROOT), placeDirectory(Campaign::Place::MEDIA_ROOT),
                                Core::MediaType::MapFile);
            break;
        case Core::CampaignDataCategory::MindMaps:
            m_editor->copyMedia(makeSource(MEDIA_ROOT), placeDirectory(Campaign::Place::MEDIA_ROOT),
                                Core::MediaType::MindmapFile);
            break;
        case Core::CampaignDataCategory::WebLink:
            m_editor->copyMedia(makeSource(MEDIA_ROOT), placeDirectory(Campaign::Place::MEDIA_ROOT),
                                Core::MediaType::WebpageFile);
            break;
        case Core::CampaignDataCategory::PDFDoc:
            m_editor->copyMedia(makeSource(MEDIA_ROOT), placeDirectory(Campaign::Place::MEDIA_ROOT),
                                Core::MediaType::PdfFile);
            break;
        case Core::CampaignDataCategory::CharacterSheets:
            m_editor->copyMedia(makeSource(MEDIA_ROOT), placeDirectory(Campaign::Place::MEDIA_ROOT),
                                Core::MediaType::CharacterSheetFile);
            break;
        case Core::CampaignDataCategory::Notes:
            m_editor->copyMedia(makeSource(MEDIA_ROOT), placeDirectory(Campaign::Place::MEDIA_ROOT),
                                Core::MediaType::TextFile);
            break;
        case Core::CampaignDataCategory::DiceAlias:
        {
            // auto dest= placeDirectory(Campaign::Place::DICE_MODEL);

            m_editor->loadDiceAlias(makeSource(DICE_ALIAS_MODEL));
        }
        break;
        case Core::CampaignDataCategory::CharacterStates:
            // m_editor->loadDiceAlias(makeSource(STATE_MODEL));
            m_editor->loadStates(makeSource(STATE_MODEL), makeSource(STATE_ROOT),
                                 placeDirectory(Campaign::Place::STATE_MODEL),
                                 placeDirectory(Campaign::Place::STATE_ROOT));
            break;
        case Core::CampaignDataCategory::Themes:
            m_editor->copyTheme(makeSource(THEME_FILE), placeDirectory(Campaign::Place::THEME_FILE));
            break;
        }
        //, Maps, MindMaps, Notes, WebLink, PDFDoc, DiceAlias, CharacterStates, Themes, CharacterSheets,
    }
}

bool CampaignManager::performAction(const QString& path, Core::CampaignAction action)
{
    bool res= false;
    // action == 0 => forget file
    auto abPath= m_editor->mediaFullPath(path);

    switch(action)
    {
    case Core::CampaignAction::NoneAction:
        break;
    case Core::CampaignAction::ForgetAction:
        res= m_editor->removeMedia(abPath);
        break;
    case Core::CampaignAction::DeleteAction:
        res= m_editor->removeFile(path);
        break;
    case Core::CampaignAction::CreateAction:
        emit createBlankFile(path, FileSerializer::typeFromExtention(path));
        res= true;
        break;
    case Core::CampaignAction::ManageAction:
        res= m_editor->addMedia(QUuid::createUuid().toString(QUuid::WithoutBraces), path, {});
        break;
    }
    // action == 1 => create document
    return res;
}

} // namespace campaign
