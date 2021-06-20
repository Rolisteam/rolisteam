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
namespace campaign
{
class Campaign;
}
namespace charactersheet
{
class ImageModel;
}
class ContentController;
namespace Settingshelper
{
void readConnectionProfileModel(ProfileModel* model);
void writeConnectionProfileModel(ProfileModel* model);
} // namespace Settingshelper

namespace ModelHelper
{
bool saveSession(const QString& path, const QString& name, const ContentController* ctrl);
QString loadSession(const QString& path, ContentController* ctrl);

bool saveCharacterSheet(const QString& path, const CharacterSheetModel* model);
bool loadCharacterSheet(const QString& path, CharacterSheetModel* model, charactersheet::ImageModel* imgModel,
                        QJsonObject& root, QString& qmlCode);

bool fetchThemeModel(const QJsonObject& themes, DiceAliasModel* model);
void fetchDiceModel(const QJsonArray& dice, DiceAliasModel* model);
void fetchMedia(const QJsonArray& medias, campaign::Campaign* campaign);

void fetchCharacterStateModel(const QJsonArray& states, CharacterStateModel* model);
QJsonArray saveCharacterStateModel(CharacterStateModel* model);

QJsonObject saveCampaign();

} // namespace ModelHelper
#endif
