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
#include "worker/modelhelper.h"

#include <QApplication>
#include <QBuffer>
#include <QDebug>
#include <QFileInfo>
#include <QFontDatabase>
#include <QJsonDocument>
#include <QMessageBox>
#include <QSettings>
#include <QUuid>

#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/imagemodel.h"
#include "controller/audiocontroller.h"
#include "controller/contentcontroller.h"
#include "data/campaign.h"
#include "data/media.h"
#include "diceparser/dicealias.h"
#include "media/mediatype.h"
#include "model/characterstatemodel.h"
#include "model/contentmodel.h"
#include "model/dicealiasmodel.h"
#include "model/historymodel.h"
#include "model/musicmodel.h"
#include "model/nonplayablecharactermodel.h"
#include "model/profilemodel.h"
#include "network/connectionprofile.h"
#include "worker/fileserializer.h"
#include "worker/iohelper.h"

namespace SettingsHelper
{
constexpr char const* rolisteam{"rolisteam"};
constexpr char const* historyGroup{"History"};
constexpr char const* historyArray{"HistoryArray"};
constexpr char const* historyCapacity{"HistoryMaxCapacity"};

constexpr char const* hist_key_id{"id"};
constexpr char const* hist_key_displayName{"displayName"};
constexpr char const* hist_key_path{"path"};
constexpr char const* hist_key_lastaccess{"access"};
constexpr char const* hist_key_bookmark{"bookmark"};
constexpr char const* hist_key_type{"type"};

const static QStringList CharacterFields({"CharacterHp", "CharacterMaxHp", "CharacterMinHp", "CharacterDistPerTurn",
                                          "CharacterStateId", "CharacterLifeColor", "CharacterInitCmd",
                                          "CharacterHasInit"});

namespace profiles
{
constexpr auto const* groupName{"ConnectionPorfiles"};
constexpr auto const* arrayName{"ConnectionProfilesArray"};
constexpr auto const* address{"address"};
constexpr auto const* name{"name"};
constexpr auto const* title{"title"};
constexpr auto const* playerId{"playerId"};
constexpr auto const* port{"port"};
constexpr auto const* server{"server"};
constexpr auto const* gm{"gm"};
constexpr auto const* campaignPath{"campaignPath"};
constexpr auto const* password{"password"};
constexpr auto const* playerColor{"PlayerColor"};
constexpr auto const* playerAvatar{"playerAvatarData"};
constexpr auto const* playerCount{"characterCount"};
} // namespace profiles

void readConnectionProfileModel(ProfileModel* model)
{
    QSettings settings(rolisteam, rolisteam);
    settings.beginGroup("ConnectionProfiles");
    int size= settings.beginReadArray("ConnectionProfilesArray");
    for(int i= 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        ConnectionProfile* profile= new ConnectionProfile();
        profile->setAddress(settings.value("address").toString());
        profile->setPlayerName(settings.value("name").toString());
        profile->setProfileTitle(settings.value("title").toString());
        profile->setPlayerId(settings.value("playerId", QUuid::createUuid().toString(QUuid::WithoutBraces)).toString());
        profile->setPort(static_cast<quint16>(settings.value("port").toInt()));
        profile->setServerMode(settings.value("server").toBool());
        profile->setGm(settings.value("gm").toBool());
        profile->setCampaignPath(settings.value("campaignPath").toString());
        profile->setHash(QByteArray::fromBase64(settings.value("password").toByteArray()));

        QColor color= settings.value("PlayerColor").value<QColor>();
        profile->setPlayerColor(color);
        profile->setPlayerAvatar(settings.value("playerAvatarData").toByteArray());

        auto characterCount= settings.beginReadArray("characterCount");
        profile->clearCharacter();
        for(int j= 0; j < characterCount; ++j)
        {
            settings.setArrayIndex(j);

            auto name= settings.value("CharacterName").toString();
            auto path= settings.value("CharacterData").toByteArray();
            auto color= settings.value("CharacterColor").value<QColor>();
            QHash<QString, QVariant> params;
            for(const auto& key : CharacterFields)
            {
                params.insert(key, settings.value(key));
            }
            profile->addCharacter(connection::CharacterData({name, color, path, params}));
        }
        settings.endArray();

        if(characterCount == 0 && !profile->isGM())
        {
            auto name= settings.value("CharacterName").toString();
            auto path= settings.value("CharacterData").toByteArray();
            auto color= settings.value("CharacterColor").value<QColor>();

            QHash<QString, QVariant> params;
            profile->addCharacter(connection::CharacterData({name, color, path, params}));
        }
        model->appendProfile(profile);
    }
    settings.endArray();
    settings.endGroup();

    if(size == 0)
    {
        ConnectionProfile* profile= new ConnectionProfile();
        model->appendProfile(profile);
    }
}

void writeConnectionProfileModel(ProfileModel* model)
{
    QSettings settings(rolisteam, rolisteam);
    settings.beginGroup("ConnectionProfiles");

    auto size= model->rowCount(QModelIndex());
    settings.beginWriteArray("ConnectionProfilesArray", size);
    for(int i= 0; i < size; ++i)
    {
        auto profile= model->getProfile(i);

        if(nullptr == profile)
            continue;

        settings.setArrayIndex(i);

        settings.setValue("address", profile->address());
        settings.setValue("name", profile->playerName());
        settings.setValue("title", profile->profileTitle());
        settings.setValue("server", profile->isServer());
        settings.setValue("port", profile->port());
        settings.setValue("playerId", profile->playerId());
        settings.setValue("gm", profile->isGM());
        settings.setValue("password", profile->password().toBase64());
        settings.setValue("PlayerColor", profile->playerColor());
        settings.setValue("playerAvatarData", profile->playerAvatar());
        settings.setValue("campaignPath", profile->campaignPath());

        settings.beginWriteArray("characterCount");
        for(int j= 0; j < profile->characterCount(); ++j)
        {
            settings.setArrayIndex(j);
            auto charact= profile->character(j);

            settings.setValue("CharacterName", charact.m_name);
            settings.setValue("CharacterData", charact.m_avatarData);
            settings.setValue("CharacterColor", charact.m_color);

            for(const auto& key : CharacterFields)
            {
                settings.setValue(key, charact.m_params[key]);
            }
        }
        settings.endArray();
    }
    settings.endArray();
    settings.endGroup();
}

void readHistoryModel(history::HistoryModel* model)
{
    QSettings settings(rolisteam, rolisteam);
    settings.beginGroup(historyGroup);
    model->setMaxCapacity(settings.value(historyCapacity, 15).toInt());
    int size= settings.beginReadArray(historyArray);
    QList<history::LinkInfo> infos;
    for(int i= 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        history::LinkInfo info{settings.value(hist_key_id).toString(),
                               settings.value(hist_key_displayName).toString(),
                               settings.value(hist_key_path).toUrl(),
                               static_cast<Core::ContentType>(settings.value(hist_key_type).toInt()),
                               settings.value(hist_key_lastaccess).toDateTime(),
                               settings.value(hist_key_bookmark).toBool()};

        infos << info;
    }
    model->setLinks(infos);
}

void writeHistoryModel(history::HistoryModel* model)
{
    QSettings settings(rolisteam, rolisteam);
    settings.beginGroup(historyGroup);
    settings.setValue(historyCapacity, model->maxCapacity());
    auto const& links= model->data();

    settings.beginWriteArray(historyArray, links.size());
    int i= 0;
    for(auto const& info : links)
    {
        settings.setArrayIndex(i);
        settings.setValue(hist_key_id, info.id);
        settings.setValue(hist_key_displayName, info.displayName);
        settings.setValue(hist_key_path, info.url);
        settings.setValue(hist_key_lastaccess, info.lastAccess);
        settings.setValue(hist_key_bookmark, info.bookmarked);
        settings.setValue(hist_key_type, static_cast<int>(info.type));
        ++i;
    }
    settings.endArray();
    settings.endGroup();
}
} // namespace SettingsHelper

namespace ModelHelper
{

bool saveSession(const ContentController* ctrl)
{
    auto model= ctrl->contentModel();
    if(model == nullptr)
        return false;

    auto ctrls= model->controllers();

    for(auto ctrl : ctrls)
    {
        if(nullptr == ctrl)
            continue;

        campaign::FileSerializer::writeFileIntoCampaign(ctrl->url().toLocalFile(), IOHelper::saveController(ctrl));
    }
    return true;
}

bool saveAudioController(const AudioController* ctrl)
{
    // TODO
    return false;
}

bool saveCharacterSheet(const QString& path, const CharacterSheetModel* model)
{
    // TODO ??
    return false;
}

bool loadCharacterSheet(const QString& path, CharacterSheetModel* model, charactersheet::ImageModel* imgModel,
                        QJsonObject& root, QString& qmlCode)
{
    if(path.isEmpty())
        return false;

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QJsonDocument json= QJsonDocument::fromJson(file.readAll());
    QJsonObject jsonObj= json.object();

    root= jsonObj["data"].toObject();

    qmlCode= jsonObj["qml"].toString();

    QJsonArray images= jsonObj["background"].toArray();

    for(auto jsonpix : images)
    {
        QJsonObject obj= jsonpix.toObject();
        QString str= obj["bin"].toString();
        QString key= obj["key"].toString();
        QString filename= obj["filename"].toString();
        bool isBg= obj["isBg"].toBool();
        QByteArray array= QByteArray::fromBase64(str.toUtf8());
        QPixmap pix;
        pix.loadFromData(array);
        imgModel->insertImage(pix, key, filename, isBg);
    }

    const auto fonts= jsonObj["fonts"].toArray();
    for(const auto& obj : fonts)
    {
        const auto font= obj.toObject();
        const auto fontData= QByteArray::fromBase64(font["data"].toString("").toLatin1());
        QFontDatabase::addApplicationFontFromData(fontData);
    }

    // m_errorList.clear();
    model->readModel(jsonObj, true);
    // displayError(m_errorList);
    return true;
}
void fetchCharacterStateModel(const QJsonArray& obj, CharacterStateModel* model)
{
    for(const auto& stateRef : obj)
    {
        auto state= stateRef.toObject();
        CharacterState da;
        da.setId(state["id"].toString());
        da.setLabel(state["label"].toString());
        auto base64= state["image"].toString();
        da.setImagePath(base64);
        QColor col;
        col.setNamedColor(state["color"].toString());
        da.setColor(col);
        model->appendState(std::move(da));
    }
}

QJsonArray saveCharacterStateModel(CharacterStateModel* model)
{
    QJsonArray states;
    auto const& statesList= model->statesList();
    for(auto const& stateInfo : statesList)
    {
        QJsonObject stateObj;
        stateObj["label"]= stateInfo->label();
        stateObj["color"]= stateInfo->color().name();
        QPixmap pix= stateInfo->imagePath();
        if(!pix.isNull())
        {
            QByteArray bytes;
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::WriteOnly);
            pix.save(&buffer, "PNG");
            stateObj["image"]= QString(buffer.data().toBase64());
        }
        states.append(stateObj);
    }
    return states;
}

void fetchDiceModel(const QJsonArray& dice, DiceAliasModel* model)
{
    for(const auto& obj : dice)
    {
        auto diceCmd= obj.toObject();

        DiceAlias alias(
            diceCmd[Core::DiceAlias::k_dice_pattern].toString(), diceCmd[Core::DiceAlias::k_dice_command].toString(),
            diceCmd[Core::DiceAlias::k_dice_comment].toString(), diceCmd[Core::DiceAlias::k_dice_replacement].toBool(),
            diceCmd[Core::DiceAlias::k_dice_enabled].toBool());

        model->appendAlias(std::move(alias));
    }
}

void fetchMedia(const QJsonArray& medias, campaign::Campaign* campaign)
{
    for(const auto& ref : medias)
    {
        auto const& obj= ref.toObject();
        auto mediaRoot= campaign->directory(campaign::Campaign::Place::MEDIA_ROOT);
        auto id= obj[Core::JsonKey::JSON_MEDIA_ID].toString();
        auto path= QString("%1/%2").arg(mediaRoot, obj[Core::JsonKey::JSON_MEDIA_PATH].toString());
        auto time= QDateTime::fromString(obj[Core::JsonKey::JSON_MEDIA_CREATIONTIME].toString(), Qt::ISODate);
        auto type= campaign::FileSerializer::typeFromExtention(path);
        QFileInfo info(path);
        auto name= info.baseName();

        std::unique_ptr<campaign::Media> media(new campaign::Media(id, name, path, type, time));
        campaign->addMedia(std::move(media));
    }
}

void fetchNpcModel(const QJsonArray& npc, campaign::NonPlayableCharacterModel* model, const QString& rootDir)
{
    std::vector<campaign::NonPlayableCharacter*> vec;
    for(const auto& obj : npc)
    {
        auto npcObj= obj.toObject();

        auto npc= IOHelper::jsonObjectToNpc(npcObj, rootDir);
        vec.push_back(npc);
    }
    model->setModelData(vec);
}

void fetchMusicModelWithTableTop(MusicModel* model)
{
    QString url= QStringLiteral("http://tabletopaudio.com/download.php?downld_file=%1");
    static QStringList list({QStringLiteral("1_The_Inner_Core.mp3"),
                             QStringLiteral("2_Bubbling_Pools.mp3"),
                             QStringLiteral("3_The_March_of_the_Faithful.mp3"),
                             QStringLiteral("4_Solemn_Vow-a.mp3"),
                             QStringLiteral("5_Desert_Bazaar.mp3"),
                             QStringLiteral("6_Abyssal_Gaze.mp3"),
                             QStringLiteral("7_The_Desert_Awaits.mp3"),
                             QStringLiteral("8_New_Dust_to_Dust.mp3"),
                             QStringLiteral("9_Before_The_Storm.mp3"),
                             QStringLiteral("10_In_The_Shadows.mp3"),
                             QStringLiteral("11_Shelter_from_the_Storm.mp3"),
                             QStringLiteral("12_Disembodied_Spirits.mp3"),
                             QStringLiteral("13_Cave_of_Time.mp3"),
                             QStringLiteral("14_Protean_Fields.mp3"),
                             QStringLiteral("15_Alien_Machine_Shop.mp3"),
                             QStringLiteral("16_Busy_Space_Port.mp3"),
                             QStringLiteral("17_Alien_Night_Club.mp3"),
                             QStringLiteral("18_House_on_the_Hill.mp3"),
                             QStringLiteral("19_Age_of_Sail.mp3"),
                             QStringLiteral("20_Dark_Continent_aa.mp3"),
                             QStringLiteral("21_Derelict_Freighter.mp3"),
                             QStringLiteral("22_True_West_a.mp3"),
                             QStringLiteral("23_The_Slaughtered_Ox.mp3"),
                             QStringLiteral("24_Forbidden_Galaxy.mp3"),
                             QStringLiteral("25_Deep_Space_EVA.mp3"),
                             QStringLiteral("26_Uncommon_Valor_a.mp3"),
                             QStringLiteral("27_Xingu_Nights.mp3"),
                             QStringLiteral("28_Nephilim_Labs_FE.mp3"),
                             QStringLiteral("29_Kaltoran_Craft_FE.mp3"),
                             QStringLiteral("30_Los_Vangeles_3030.mp3"),
                             QStringLiteral("31_Frozen_Wastes.mp3"),
                             QStringLiteral("32_City_and_the_City.mp3"),
                             QStringLiteral("33_Far_Above_the_World.mp3"),
                             QStringLiteral("34_Clash_of_Kings.mp3"),
                             QStringLiteral("35_Swamplandia.mp3"),
                             QStringLiteral("36_Down_by_the_Sea.mp3"),
                             QStringLiteral("37_Catacombs.mp3"),
                             QStringLiteral("38_Into_the_Deep.mp3"),
                             QStringLiteral("39_Temple_of_the_Eye.mp3"),
                             QStringLiteral("40_The_Long_Rain.mp3"),
                             QStringLiteral("41_Starship_Bridge.mp3"),
                             QStringLiteral("42_Rise_of_the_Ancients.mp3"),
                             QStringLiteral("43_Dome_City_Center.mp3"),
                             QStringLiteral("44_Victorian_London.mp3"),
                             QStringLiteral("45_Samurai_HQ.mp3"),
                             QStringLiteral("46_Cathedral.mp3"),
                             QStringLiteral("47_There_be_Dragons.mp3"),
                             QStringLiteral("48_Overland_with_Oxen.mp3"),
                             QStringLiteral("49_Goblin's_Cave.mp3"),
                             QStringLiteral("50_Super_Hero.mp3"),
                             QStringLiteral("51_Woodland_Campsite.mp3"),
                             QStringLiteral("52_Warehouse_13.mp3"),
                             QStringLiteral("53_Strangers_on_a_Train.mp3"),
                             QStringLiteral("54_Mountain_Tavern.mp3"),
                             QStringLiteral("55_Ice_Cavern.mp3"),
                             QStringLiteral("56_Medieval_Town.mp3"),
                             QStringLiteral("57_Colosseum.mp3"),
                             QStringLiteral("58_Terror.mp3"),
                             QStringLiteral("59_Dinotopia.mp3"),
                             QStringLiteral("60_Dark_and_Stormy.mp3"),
                             QStringLiteral("61_Orbital_Platform.mp3"),
                             QStringLiteral("62_Middle_Earth_Dawn.mp3"),
                             QStringLiteral("63_Industrial_Shipyard.mp3"),
                             QStringLiteral("64_Mountain_Pass.mp3"),
                             QStringLiteral("65_Dungeon_I.mp3"),
                             QStringLiteral("66_Royal_Salon.mp3"),
                             QStringLiteral("67_Asylum.mp3"),
                             QStringLiteral("68_1940s_Office.mp3"),
                             QStringLiteral("69_Forest_Night.mp3"),
                             QStringLiteral("70_Age_of_Steam.mp3")});

    QList<QUrl> urls;

    for(auto const& song : qAsConst(list))
    {
        urls.append(QUrl::fromUserInput(url.arg(song)));
    }

    model->addSong(urls);
}

} // namespace ModelHelper
