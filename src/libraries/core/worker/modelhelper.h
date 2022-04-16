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

class SessionItemModel;
class ProfileModel;
class CharacterSheetModel;
class DiceAliasModel;
class CharacterStateModel;
class AudioController;
class MusicModel;
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
void readConnectionProfileModel(ProfileModel* model);
void writeConnectionProfileModel(ProfileModel* model);

void readHistoryModel(history::HistoryModel* model);
void writeHistoryModel(history::HistoryModel* model);

} // namespace SettingsHelper

namespace ModelHelper
{
bool saveSession(const ContentController* ctrl);
bool saveAudioController(const AudioController* ctrl);

bool saveCharacterSheet(const QString& path, const CharacterSheetModel* model);
bool loadCharacterSheet(const QString& path, CharacterSheetModel* model, charactersheet::ImageModel* imgModel,
                        QJsonObject& root, QString& qmlCode);

bool fetchThemeModel(const QJsonObject& themes, DiceAliasModel* model);
void fetchDiceModel(const QJsonArray& dice, DiceAliasModel* model);
void fetchMedia(const QJsonArray& medias, campaign::Campaign* campaign);
void fetchNpcModel(const QJsonArray& npcs, campaign::NonPlayableCharacterModel* model, const QString& rootDir);

void fetchCharacterStateModel(const QJsonArray& states, CharacterStateModel* model);
QJsonArray saveCharacterStateModel(CharacterStateModel* model);

void fetchMusicModelWithTableTop(MusicModel* model);

QJsonObject saveCampaign();

} // namespace ModelHelper
#endif
