/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#ifndef CORE_MODEL_HELPER_H
#define CORE_MODEL_HELPER_H
#include <QJsonObject>
#include <QString>
#include <core_global.h>
class SessionItemModel;
class ProfileModel;
class CharacterSheetModel;
class DiceAliasModel;
class CharacterStateModel;
class AudioController;
class MusicModel;
class Dice3DController;
namespace campaign
{
class Campaign;
class NonPlayableCharacterModel;
} // namespace campaign
namespace charactersheet
{
class ImageModel;
}
namespace history
{
class HistoryModel;
}
class ContentController;
namespace SettingsHelper
{
CORE_EXPORT void readConnectionProfileModel(ProfileModel* model);
CORE_EXPORT void writeConnectionProfileModel(ProfileModel* model);

CORE_EXPORT void readHistoryModel(history::HistoryModel* model);
CORE_EXPORT void writeHistoryModel(history::HistoryModel* model);

} // namespace SettingsHelper

namespace ModelHelper
{
CORE_EXPORT bool saveSession(const ContentController* ctrl);
CORE_EXPORT QByteArray buildDice3dData(Dice3DController* ctrl);
CORE_EXPORT bool saveDice3d(Dice3DController* ctrl, const QString& destination);
CORE_EXPORT bool fetchDice3d(Dice3DController* ctrl, const QByteArray& data);

// CORE_EXPORT bool saveCharacterSheet(const QString& path, const CharacterSheetModel* model);
CORE_EXPORT bool loadCharacterSheet(const QString& path, CharacterSheetModel* model,
                                    charactersheet::ImageModel* imgModel, QJsonObject& root, QString& qmlCode);

CORE_EXPORT bool fetchThemeModel(const QJsonObject& themes, DiceAliasModel* model);
CORE_EXPORT void fetchDiceModel(const QJsonArray& dice, DiceAliasModel* model);
CORE_EXPORT void fetchMedia(const QJsonArray& medias, campaign::Campaign* campaign);
CORE_EXPORT void fetchNpcModel(const QJsonArray& npcs, campaign::NonPlayableCharacterModel* model,
                               const QString& rootDir);

CORE_EXPORT void fetchCharacterStateModel(const QJsonArray& states, CharacterStateModel* model);
CORE_EXPORT QJsonArray saveCharacterStateModel(CharacterStateModel* model);

CORE_EXPORT void fetchMusicModelWithTableTop(MusicModel* model);

// CORE_EXPORT QJsonObject saveCampaign();

} // namespace ModelHelper
#endif
