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

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QStringList>

#include <memory>
#include <vector>

#include "media/mediatype.h"

class DiceAlias;
class CharacterState;
namespace campaign
{
class CampaignManager;
struct CampaignInfo
{
    bool status;
    QJsonObject asset;
    QJsonObject theme;
    QJsonArray dices;
    QJsonArray states;
    QStringList errors;
    QStringList unmanagedFiles;
    QStringList missingFiles;
};

class FileSerializer
{
public:
    FileSerializer();
    static Core::MediaType typeFromExtention(const QString& filename);
    static CampaignInfo readCampaignDirectory(const QString& directory);

    static QJsonArray statesToArray(const std::vector<std::unique_ptr<CharacterState>>& vec,
                                    const QString& destination);
    static QJsonArray dicesToArray(const std::vector<std::unique_ptr<DiceAlias>>& vec);

    static void writeStatesIntoCampaign(const QString& destination, const QJsonArray& array);
    static void writeDiceAliasIntoCampaign(const QString& destination, const QJsonArray& array);
};
} // namespace campaign

#endif // CORE_FILESERIALIZER_H
