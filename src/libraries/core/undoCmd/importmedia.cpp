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
#include "importmedia.h"

#include <QFileInfo>

#include "data/campaign.h"
#include "data/media.h"
#include "worker/fileserializer.h"
#include "worker/iohelper.h"

namespace commands
{
ImportMedia::ImportMedia(campaign::Campaign* campaign, const QString& name, const QString& sourcePath,
                         const QString& destPath)
    : m_campaign(campaign)
    , m_uuid(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_name(name)
    , m_tmpPath(QString("%1/%2/%3").arg(campaign->rootDirectory(), campaign::TRASH_FOLDER, m_uuid))
    , m_destPath(destPath)
{
    QFileInfo info(sourcePath);
    setText(tr("Import media: %1").arg(info.fileName()));
    IOHelper::makeDir(m_destPath);
    m_destPath= QString("%1/%2").arg(m_destPath, info.fileName());
    IOHelper::copyFile(sourcePath, m_tmpPath);
}

void ImportMedia::redo()
{
    auto it= std::unique_ptr<campaign::Media>(
        new campaign::Media(m_uuid, m_name, m_destPath, campaign::FileSerializer::typeFromExtention(m_destPath)));
    IOHelper::moveFile(m_tmpPath, m_destPath);
    m_campaign->addMedia(std::move(it));
}

void ImportMedia::undo()
{
    IOHelper::moveFile(m_destPath, m_tmpPath);
    m_campaign->removeMedia(m_uuid);
}

QString ImportMedia::destination() const
{
    return m_destPath;
}

} // namespace commands
