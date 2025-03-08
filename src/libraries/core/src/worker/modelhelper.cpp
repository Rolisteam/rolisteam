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
#include "dice3dcontroller.h"
#include "diceparser/dicealias.h"
#include "media/mediatype.h"
#include "model/characterstatemodel.h"
#include "model/contentmodel.h"
#include "model/dicealiasmodel.h"
#include "model/historymodel.h"
#include "model/instantmessagingmodel.h"
#include "model/musicmodel.h"
#include "model/nonplayablecharactermodel.h"
#include "model/profilemodel.h"
#include "network/connectionprofile.h"
#include "utils/iohelper.h"
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
                                          "CharacterInit"
                                          "CharacterHasInit"});

namespace profiles
{
constexpr auto const* groupName{"ConnectionProfiles"};
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
constexpr auto const* characterCount{"characterCount"};
constexpr auto const* characterName{"CharacterName"};
constexpr auto const* characterId{"CharacterId"};
constexpr auto const* characterData{"CharacterData"};
constexpr auto const* characterColor{"CharacterColor"};
} // namespace profiles

namespace jsonkey3DDice
{
constexpr auto colorD4{"colorD4"};
constexpr auto colorD6{"colorD6"};
constexpr auto colorD8{"colorD8"};
constexpr auto colorD10{"colorD10"};
constexpr auto colorD12{"colorD12"};
constexpr auto colorD20{"colorD20"};
constexpr auto colorD100{"colorD100"};
constexpr auto displayed{"displayed"};
constexpr auto muted{"muted"};
constexpr auto commandPart{"commandPart"};
constexpr auto shared{"sharedOnline"};
constexpr auto hideTime{"hideTime"};
constexpr auto model{"model"};
constexpr auto factor{"factor"};
} // namespace jsonkey3DDice

void readConnectionProfileModel(ProfileModel* model)
{
#ifdef QT_DEBUG
    QSettings settings(QString("bla_%1").arg(QDate::currentDate().toString("MM-YYYY")),
                       QString("bla_%1").arg(QDate::currentDate().toString("MM-YYYY")));
#else
    QSettings settings(rolisteam, rolisteam);
#endif
    qDebug() << settings.fileName();
    settings.beginGroup(profiles::groupName);
    int size= settings.beginReadArray(profiles::arrayName);
    for(int i= 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        ConnectionProfile* profile= new ConnectionProfile();
        profile->setAddress(settings.value(profiles::address).toString());
        profile->setPlayerName(settings.value(profiles::name).toString());
        profile->setProfileTitle(settings.value(profiles::title).toString());
        profile->setPlayerId(
            settings.value(profiles::playerId, QUuid::createUuid().toString(QUuid::WithoutBraces)).toString());
        profile->setPort(static_cast<quint16>(settings.value(profiles::port).toInt()));
        profile->setServerMode(settings.value(profiles::server).toBool());
        profile->setGm(settings.value(profiles::gm).toBool());
        profile->setCampaignPath(settings.value(profiles::campaignPath).toString());
        profile->setHash(QByteArray::fromBase64(settings.value(profiles::password).toByteArray()));

        QColor color= settings.value(profiles::playerColor).value<QColor>();
        profile->setPlayerColor(color);
        profile->setPlayerAvatar(settings.value(profiles::playerAvatar).toByteArray());

        auto characterCount= settings.beginReadArray(profiles::characterCount);
        profile->clearCharacter();
        for(int j= 0; j < characterCount; ++j)
        {
            settings.setArrayIndex(j);

            auto uuid= settings.value(profiles::characterId, QUuid::createUuid().toString()).toString();
            auto name= settings.value(profiles::characterName).toString();
            auto path= settings.value(profiles::characterData).toByteArray();
            auto color= settings.value(profiles::characterColor).value<QColor>();
            QHash<QString, QVariant> params;
            for(const auto& key : CharacterFields)
            {
                params.insert(key, settings.value(key));
            }
            profile->addCharacter(connection::CharacterData({uuid, name, color, path, params}));
        }
        settings.endArray();

        if(characterCount == 0 && !profile->isGM())
        {
            auto uuid= settings.value(profiles::characterId, QUuid::createUuid().toString()).toString();
            auto name= settings.value(profiles::characterName).toString();
            auto path= settings.value(profiles::characterData).toByteArray();
            auto color= settings.value(profiles::characterColor).value<QColor>();

            QHash<QString, QVariant> params;
            profile->addCharacter(connection::CharacterData({uuid, name, color, path, params}));
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
#ifdef QT_DEBUG
    QSettings settings(QString("bla_%1").arg(QDate::currentDate().toString("MM-YYYY")),
                       QString("bla_%1").arg(QDate::currentDate().toString("MM-YYYY")));
#else
    QSettings settings(rolisteam, rolisteam);
#endif
    settings.beginGroup(profiles::groupName);

    auto size= model->rowCount(QModelIndex());
    settings.beginWriteArray(profiles::arrayName, size);
    for(int i= 0; i < size; ++i)
    {
        auto profile= model->getProfile(i);

        if(nullptr == profile)
            continue;

        settings.setArrayIndex(i);

        settings.setValue(profiles::address, profile->address());
        settings.setValue(profiles::name, profile->playerName());
        settings.setValue(profiles::title, profile->profileTitle());
        settings.setValue(profiles::server, profile->isServer());
        settings.setValue(profiles::port, profile->port());
        settings.setValue(profiles::playerId, profile->playerId());
        settings.setValue(profiles::gm, profile->isGM());
        settings.setValue(profiles::password, profile->password().toBase64());
        settings.setValue(profiles::playerColor, profile->playerColor());
        settings.setValue(profiles::playerAvatar, profile->playerAvatar());
        settings.setValue(profiles::campaignPath, profile->campaignPath());

        settings.beginWriteArray(profiles::characterCount);
        for(int j= 0; j < profile->characterCount(); ++j)
        {
            settings.setArrayIndex(j);
            auto charact= profile->character(j);

            settings.setValue(profiles::characterId, charact.m_uuid);
            settings.setValue(profiles::characterName, charact.m_name);
            settings.setValue(profiles::characterData, charact.m_avatarData);
            settings.setValue(profiles::characterColor, charact.m_color);

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

QByteArray buildDice3dData(Dice3DController* ctrl)
{
    namespace sj= SettingsHelper::jsonkey3DDice;

    QJsonObject obj;

    obj[sj::colorD4]= ctrl->fourColor().name();
    obj[sj::colorD6]= ctrl->sixColor().name();
    obj[sj::colorD8]= ctrl->eightColor().name();
    obj[sj::colorD10]= ctrl->tenColor().name();
    obj[sj::colorD12]= ctrl->twelveColor().name();
    obj[sj::colorD20]= ctrl->twentyColor().name();
    obj[sj::colorD100]= ctrl->oneHundredColor().name();

    obj[sj::displayed]= ctrl->displayed();
    obj[sj::factor]= ctrl->factor();
    obj[sj::muted]= ctrl->muted();
    obj[sj::commandPart]= ctrl->commandPart();
    obj[sj::shared]= ctrl->sharedOnline();
    obj[sj::hideTime]= ctrl->hideTime();
    auto const& model= ctrl->model()->localModel();
    QList<DiceController*> dices;
    std::transform(std::begin(model), std::end(model), std::back_inserter(dices),
                   [](const std::unique_ptr<DiceController>& ctrl) { return ctrl.get(); });
    auto bytes= DiceModel::diceControllerToData(dices, QSize{2, 2});
    auto doc= QJsonDocument::fromJson(bytes);
    obj[sj::model]= doc.array();

    QJsonDocument output;
    output.setObject(obj);
    return output.toJson();
}

bool saveDice3d(Dice3DController* ctrl, const QString& destination)
{
    campaign::FileSerializer::writeFileIntoCampaign(destination, buildDice3dData(ctrl));
    return true;
}

bool fetchDice3d(Dice3DController* ctrl, const QByteArray& data)
{
    if(data.isEmpty())
        return false;

    auto docu= QJsonDocument::fromJson(data);

    auto obj= docu.object();
    namespace sj= SettingsHelper::jsonkey3DDice;
    ctrl->setFourColor(obj[sj::colorD4].toString());
    ctrl->setSixColor(obj[sj::colorD6].toString());
    ctrl->setEightColor(obj[sj::colorD8].toString());
    ctrl->setTenColor(obj[sj::colorD10].toString());
    ctrl->setTwelveColor(obj[sj::colorD12].toString());
    ctrl->setTwentyColor(obj[sj::colorD20].toString());
    ctrl->setOneHundredColor(obj[sj::colorD100].toString());
    ctrl->setDisplayed(obj[sj::displayed].toBool(false));
    ctrl->setMuted(obj[sj::muted].toBool(false));
    ctrl->setCommandPart(obj[sj::commandPart].toString());
    ctrl->setSharedOnline(obj[sj::shared].toBool(false));
    ctrl->setFactor(obj[sj::factor].toDouble(32.0));
    ctrl->setHideTime(obj[sj::hideTime].toInt());

    auto array= obj[sj::model].toArray();
    std::vector<std::unique_ptr<DiceController>> temp;
    DiceModel::fetchModel(IOHelper::jsonArrayToByteArray(array), temp, QSize{2, 2});
    auto model= ctrl->model();

    for(auto& dice : temp)
    {
        model->addDice(dice.release());
    }
    return true;
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
void fetchCharacterStateModel(const QJsonArray& obj, CharacterStateModel* model, const QString& rootDir)
{
    for(const auto& stateRef : obj)
    {
        auto state= stateRef.toObject();
        CharacterState da;
        da.setId(state["id"].toString());
        da.setLabel(state["label"].toString());
        auto path= state["image"].toString();
        if(!path.isEmpty())
            da.setImagePath(QString("%2/%1").arg(path).arg(rootDir));
        else
            da.setImagePath(QString());

        QColor col= QColor::fromString(state["color"].toString());
        da.setColor(col);
        model->appendState(std::move(da));
    }

    auto const& states= model->statesList();
    auto list= new QList<CharacterState*>();
    std::transform(std::begin(states), std::end(states), std::back_inserter(*list),
                   [](const std::unique_ptr<CharacterState>& item) { return item.get(); });
    Character::setListOfCharacterState(list);
}

QJsonArray saveCharacterStateModel(CharacterStateModel* model)
{
    QJsonArray states;
    auto const& statesList= model->statesList();
    for(auto const& stateInfo : statesList)
    {
        QJsonObject stateObj;
        stateObj["id"]= stateInfo->id();
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

    for(auto const& song : std::as_const(list))
    {
        urls.append(QUrl::fromUserInput(url.arg(song)));
    }

    model->addSong(urls);
}

namespace sim
{
constexpr auto type{"type"};
constexpr auto title{"title"};
constexpr auto localId{"localId"};
constexpr auto id{"id"};
constexpr auto recipiants{"recipiants"};
constexpr auto messages{"messages"};
constexpr auto rooms{"rooms"};

constexpr auto msgType{"type"};
constexpr auto msgText{"text"};
constexpr auto msgTime{"fullTime"};
constexpr auto msgOwner{"ownerId"};
constexpr auto msgWriter{"writerId"};
constexpr auto msgLink{"imageLink"};

} // namespace sim

void fetchInstantMessageModel(const QJsonObject& obj, InstantMessaging::InstantMessagingModel* model)
{
    model->clear();
    auto localId= obj[sim::localId].toString();
    model->setLocalId(localId);
    auto array= obj[sim::rooms].toArray();
    for(auto room : array)
    {
        auto roomJson= room.toObject();
        auto type= static_cast<InstantMessaging::ChatRoom::ChatRoomType>(roomJson[sim::type].toInt());
        auto title= roomJson[sim::title].toString();

        auto localId= roomJson[sim::localId].toString();
        Q_UNUSED(localId)
        auto id= roomJson[sim::id].toString();
        auto list= roomJson[sim::recipiants].toArray();
        QStringList recipiants;
        std::transform(std::begin(list), std::end(list), std::back_inserter(recipiants),
                       [](const QJsonValue& val) { return val.toString(); });

        switch(type)
        {
        case InstantMessaging::ChatRoom::GLOBAL:
            model->insertGlobalChatroom(title, id);
            break;
        case InstantMessaging::ChatRoom::SINGLEPLAYER:
            model->insertIndividualChatroom(id, title);
            break;
        case InstantMessaging::ChatRoom::EXTRA:
            model->insertExtraChatroom(title, recipiants, false, id);
            break;
        }

        auto chat= model->chatRoomFromId(id);

        if(!chat)
            continue;
        auto msgModel= chat->messageModel();

        auto roles= msgModel->roleNames();
        auto messages= roomJson[sim::messages].toArray();
        QJsonArray msgArray;
        for(auto msgRef : messages)
        {
            auto msg= msgRef.toObject();

            auto link= QUrl::fromUserInput(msg[sim::msgLink].toString());
            auto text= msg[sim::msgText].toString();
            auto date= QDateTime::fromSecsSinceEpoch(msg[sim::msgTime].toInteger());
            auto owner= msg[sim::msgOwner].toString();
            auto writerId= msg[sim::msgWriter].toString();
            auto type= static_cast<InstantMessaging::MessageInterface::MessageType>(msg[sim::msgType].toInt());

            msgModel->addMessage(text, link, date, owner, writerId, type);
        }
        roomJson[sim::messages]= msgArray;
    }
}

QJsonObject saveInstantMessageModel(InstantMessaging::InstantMessagingModel* model)
{
    if(!model)
        return {};
    QJsonObject obj;

    auto const& rooms= model->rooms();
    QJsonArray roomArray;
    static QSet<QString> allowed{sim::msgType, sim::msgText, sim::msgTime, sim::msgOwner, sim::msgWriter, sim::msgLink};

    for(auto const& room : rooms)
    {
        QJsonObject roomJson;
        auto recipiants= room->recipiants();
        auto messages= room->messageModel();
        roomJson[sim::type]= static_cast<int>(room->type());
        roomJson[sim::title]= room->title();
        roomJson[sim::localId]= room->localId();
        roomJson[sim::id]= room->uuid();

        QStringList ids;
        if(recipiants)
            ids= recipiants->recipiantIds();

        roomJson[sim::recipiants]= QJsonArray::fromStringList(ids);

        auto roles= messages->roleNames();
        QJsonArray msgArray;
        for(auto i= 0; i < messages->rowCount(); ++i)
        {
            auto idx= messages->index(i, 0);
            QJsonObject message;
            for(auto [k, v] : roles.asKeyValueRange())
            {
                if(allowed.contains(v))
                    message[v]= messages->data(idx, k).toJsonValue();
            }
            msgArray.append(message);
        }
        roomJson[sim::messages]= msgArray;

        roomArray.append(roomJson);
    }

    obj[sim::rooms]= roomArray;
    obj[sim::localId]= model->localId();

    return obj;
}

} // namespace ModelHelper
