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
#include "campaignmanager.h"

#include "data/media.h"
#include "model/characterstatemodel.h"
#include "model/dicealiasmodel.h"
#include "undoCmd/importdatamedia.h"
#include "undoCmd/importmedia.h"
#include "worker/fileserializer.h"
#include "worker/modelhelper.h"

#include "campaigneditor.h"
#include "updater/media/campaignupdater.h"

#include <QDir>
#include <QUrl>

namespace campaign
{
CampaignManager::CampaignManager(DiceParser* diceparser, QObject* parent)
    : QObject(parent)
    , m_editor(new CampaignEditor)
    , m_campaignUpdater(new CampaignUpdater(diceparser, m_editor->campaign()))
{
    connect(m_editor.get(), &CampaignEditor::importedFile, this, &CampaignManager::fileImported);
}

CampaignManager::~CampaignManager()= default;

bool CampaignManager::createCampaign(const QUrl& dirUrl)
{
    auto dirPath= dirUrl.toLocalFile();
    auto dir= QDir(dirPath);

    if(QDir(dirPath).exists())
    {
        emit errorOccured(QString("'%1' alredy exists").arg(dirPath));
        return false;
    }

    auto parentDir= dir;
    parentDir.cdUp();
    if(!parentDir.mkdir(dirPath))
    {
        emit errorOccured(QString("Could not create '%1'").arg(dirPath));
        return false;
    }

    m_editor.reset(new CampaignEditor());
    m_editor->createNew(dirPath);
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

    auto destPath= QString("%1/%2").arg(campaign->directory(Campaign::Place::MEDIA_ROOT), name);

    auto cmd= new commands::ImportDataMedia(m_editor->campaign(), name, data, destPath);
    auto res= cmd->destination();
    m_editor->doCommand(cmd);

    return res;
}

void CampaignManager::saveCampaign()
{
    if(!m_editor)
        return;
    m_editor->save(m_editor->campaignDir());
}

void CampaignManager::openCampaign(const QUrl& dir)
{
    auto path= dir.toLocalFile();
    m_editor->open(path, false);
    Q_EMIT campaignChanged(path);
}

Campaign* CampaignManager::campaign() const
{
    return m_editor->campaign();
}

CampaignEditor* CampaignManager::editor() const
{
    return m_editor.get();
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
} // namespace campaign
