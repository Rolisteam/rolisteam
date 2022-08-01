/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
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
#ifndef CORE_FILESERIALIZER_H
#define CORE_FILESERIALIZER_H

#include <QFuture>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QStringList>

#include <memory>
#include <vector>

#include "media/mediatype.h"
#include <core_global.h>
class DiceAlias;
class CharacterState;
namespace campaign
{
class CampaignManager;
class Campaign;
class NonPlayableCharacter;
struct CORE_EXPORT CampaignInfo
{
    bool status;
    QJsonObject asset;
    QJsonObject theme;
    QJsonArray dices;
    QJsonArray states;
    QJsonArray npcs;
    QStringList errors;
    QStringList unmanagedFiles;
    QStringList missingFiles;
};

class CORE_EXPORT FileSerializer
{
public:
    FileSerializer();
    static Core::MediaType typeFromExtention(const QString& filename);
    static CampaignInfo readCampaignDirectory(const QString& directory);

    static QJsonObject campaignToObject(Campaign* campaign);

    static QJsonArray statesToArray(const std::vector<std::unique_ptr<CharacterState>>& vec,
                                    const QString& destination);
    static QJsonArray dicesToArray(const std::vector<std::unique_ptr<DiceAlias>>& vec);
    static QJsonArray npcToArray(const std::vector<std::unique_ptr<campaign::NonPlayableCharacter>>& vec,
                                 const QString& destination);

    static void writeStatesIntoCampaign(const QString& destination, const QJsonArray& array);
    static void writeDiceAliasIntoCampaign(const QString& destination, const QJsonArray& array);
    static void writeCampaignInfo(const QString& destination, const QJsonObject& object);
    static void writeNpcIntoCampaign(const QString& destination, const QJsonArray& array);
    static bool createCampaignDirectory(const QString& path);
    static QFuture<bool> writeFileIntoCampaign(const QString& destination, const QByteArray& array);

    static QString contentTypeToDefaultExtension(Core::ContentType type);
    static QString addExtention(const QString& name, Core::ContentType);

    static bool isValidCampaignDirectory(const QString& path, bool acceptEmpty= true);
    static bool hasContent(const QString& path, Core::CampaignDataCategory category);
};
} // namespace campaign

#endif // CORE_FILESERIALIZER_H
