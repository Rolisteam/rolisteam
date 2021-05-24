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
    auto assets= info.asset;
    manager->setName(assets["name"].toString());
    manager->setCurrentChapter(assets["currentChapter"].toString());
    manager->setCurrentTheme(IOHelper::jsonToTheme(info.theme));
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

void CampaignEditor::createNew(const QString& dir) {}

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

bool CampaignEditor::saveCopy(const QString& to) {}

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
