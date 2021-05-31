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
#include "modelhelper.h"

#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/imagemodel.h"
#include "controller/contentcontroller.h"
#include "data/campaign.h"
#include "data/character.h"
#include "data/media.h"
#include "data/player.h"
#include "media/mediatype.h"
#include "model/characterstatemodel.h"
#include "model/contentmodel.h"
#include "model/dicealiasmodel.h"
#include "model/palettemodel.h"
#include "model/profilemodel.h"
#include "network/connectionprofile.h"
#include "session/sessionitemmodel.h"
#include "worker/fileserializer.h"
#include "worker/iohelper.h"

#include <QApplication>
#include <QBuffer>
#include <QDebug>
#include <QFileInfo>
#include <QFontDatabase>
#include <QJsonDocument>
#include <QMessageBox>
#include <QSettings>
#include <QUuid>

namespace Settingshelper
{
const static QStringList CharacterFields({"CharacterHp", "CharacterMaxHp", "CharacterMinHp", "CharacterDistPerTurn",
                                          "CharacterStateId", "CharacterLifeColor", "CharacterInitCmd",
                                          "CharacterHasInit"});
void readConnectionProfileModel(ProfileModel* model)
{
    QSettings settings("rolisteam", "rolisteam");
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
        profile->setPlayerAvatar(settings.value("playerAvatarPath").toString());

        auto characterCount= settings.beginReadArray("characterCount");
        profile->clearCharacter();
        for(int j= 0; j < characterCount; ++j)
        {
            settings.setArrayIndex(j);

            auto name= settings.value("CharacterName").toString();
            auto path= settings.value("CharacterPath").toString();
            auto color= settings.value("CharacterColor").value<QColor>();
            QHash<QString, QVariant> params;
            for(const auto& key : CharacterFields)
            {
                params.insert(key, settings.value(key));
            }
            profile->addCharacter(CharacterData({name, color, path, params}));
        }
        settings.endArray();

        if(characterCount == 0 && !profile->isGM())
        {
            auto name= settings.value("CharacterName").toString();
            auto path= settings.value("CharacterPath").toString();
            auto color= settings.value("CharacterColor").value<QColor>();
            qDebug() << name << path;
            QHash<QString, QVariant> params;
            profile->addCharacter(CharacterData({name, color, path, params}));
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
    QSettings settings("rolisteam", "rolisteam");
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
        settings.setValue("playerAvatarPath", profile->playerAvatar());
        settings.setValue("campaignPath", profile->campaignPath());

        settings.beginWriteArray("characterCount");
        for(int j= 0; j < profile->characterCount(); ++j)
        {
            settings.setArrayIndex(j);
            auto charact= profile->character(j);

            settings.setValue("CharacterName", charact.m_name);
            settings.setValue("CharacterPath", charact.m_avatarPath);
            settings.setValue("CharacterColor", charact.m_color);

            for(const auto& key : CharacterFields)
            {
                settings.setValue(key, charact.m_params[key]);
            }
        }
        settings.endArray();

        /*    settings.setValue("CharacterColor", character->getColor());
            QImage img= character->getAvatar();
            QVariant var;
            var.setValue(img);
            settings.setValue("CharacterPix", var);
            settings.setValue("CharacterName", character->name());
            settings.setValue("CharacterPath", character->avatarPath());
            settings.setValue("CharacterHp", character->getHealthPointsCurrent());
            settings.setValue("CharacterMaxHp", character->getHealthPointsMax());
            settings.setValue("CharacterMinHp", character->getHealthPointsMin());
            settings.setValue("CharacterDistPerTurn", character->getDistancePerTurn());
            settings.setValue("CharacterStateId", character->stateId());
            settings.setValue("CharacterLifeColor", character->getLifeColor());
            settings.setValue("CharacterInitCmd", character->getInitCommand());
            settings.setValue("CharacterHasInit", character->hasInitScore());*/

        //++i;
    }
    settings.endArray();
    settings.endGroup();
}
} // namespace Settingshelper

namespace ModelHelper
{

bool saveSession(const QString& path, const QString& name, const ContentController* ctrl)
{
    if(path.isEmpty())
        return false;

    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_7);
    out << name;

    auto model= ctrl->contentModel();
    if(model == nullptr)
        return false;

    auto ctrls= model->controllers();
    out << static_cast<quint64>(ctrls.size());
    for(auto ctrl : ctrls)
    {
        out << IOHelper::saveController(ctrl);
    }
    return true;
}

QString loadSession(const QString& path, ContentController* ctrl)
{

    QFileInfo info(path);

    auto name= QStringLiteral("%1_back.sce").arg(info.baseName());
    auto backUpPath= QStringLiteral("%1/%2").arg(info.absolutePath()).arg(name);

    QFileInfo backUp(backUpPath);
    auto finalPath= path;
    if(backUp.exists() && backUp.lastModified() > info.lastModified())
    {
        auto answer= QMessageBox::question(
            qApp->activeWindow(), QObject::tr("Load Backup ?"),
            QObject::tr(
                "A backup file has been found and seem more recent than the original file. Load the back up ?"));
        if(answer == QMessageBox::Yes)
            finalPath= backUpPath;
    }

    name= info.baseName();
    QFile file(finalPath);
    if(!file.open(QIODevice::ReadOnly))
    {
        return {};
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_7);

    in >> name;
    auto model= ctrl->contentModel();

    quint64 size;
    in >> size;

    for(quint64 i= 0; i < size; ++i)
    {
        QByteArray data;
        in >> data;

        auto ctrl= IOHelper::loadController(data);

        model->appendMedia(ctrl);
    }

    return name;
}

bool saveCharacterSheet(const QString& path, const CharacterSheetModel* model)
{
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
        da.setLabel(state["label"].toString());
        // da->setIsLocal(state["local"].toBool());
        auto base64= state["image"].toString();
        da.setImagePath(base64);
        /*if(!base64.isEmpty())
        {
            QByteArray array= QByteArray::fromBase64(base64.toUtf8());
            QPixmap pix;
            pix.loadFromData(array);
            da.setImage(pix);
        }*/
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

        DiceAlias alias(diceCmd["pattern"].toString(), diceCmd["command"].toString(), diceCmd["comment"].toString(),
                        diceCmd["replace"].toBool(), diceCmd["enabled"].toBool());

        model->appendAlias(std::move(alias));
    }
}

void fetchMedia(const QJsonArray& medias, campaign::Campaign* campaign)
{
    for(const auto& obj : medias)
    {
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

} // namespace ModelHelper
