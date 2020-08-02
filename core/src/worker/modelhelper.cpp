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
#include "data/character.h"
#include "data/player.h"
#include "model/profilemodel.h"
#include "network/connectionprofile.h"
#include "session/sessionitemmodel.h"
#include "worker/iohelper.h"

#include <QDebug>
#include <QFileInfo>
#include <QFontDatabase>
#include <QJsonDocument>
#include <QSettings>
#include <QUuid>

namespace Settingshelper
{
const static QStringList CharacterFields({"CharacterHp", "CharacterMaxHp", "CharacterMinHp", "CharacterDistPerTurn",
                                          "CharacterStateId", "CharacterLifeColor", "CharacterInitCmd",
                                          "CharacterHasInit"});
void readConnectionProfileModel(ProfileModel* model)
{
    /// TODO change rolisteam1.10 to rolisteam
    QSettings settings("rolisteam", "rolisteam1.10");
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
            for(auto key : CharacterFields)
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
    /// TODO change rolisteam1.10 to rolisteam
    QSettings settings("rolisteam", "rolisteam1.10");
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

        settings.beginWriteArray("characterCount");
        for(int j= 0; j < profile->characterCount(); ++j)
        {
            settings.setArrayIndex(j);
            auto charact= profile->character(j);

            settings.setValue("CharacterName", charact.m_name);
            settings.setValue("CharacterPath", charact.m_avatarPath);
            settings.setValue("CharacterColor", charact.m_color);

            for(auto key : CharacterFields)
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
    /*QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_7);
    out << name;

    auto managers= ctrl->mediaManagers();

    for(auto manager : managers)
    {
        out << IOHelper::saveManager(manager);
    }

    auto model= ctrl->sessionModel();
    if(!model)
        return false;
    model->saveModel(out);*/

    return true;
}

QString loadSession(const QString& path, ContentController* ctrl)
{
    QString name;
    /*QFileInfo info(path);
    name= info.baseName();
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        return {};
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_7);

    in >> name;
    auto managers= ctrl->mediaManagers();

    for(auto manager : managers)
    {
        IOHelper::loadManager(manager, in);
    }

    auto model= ctrl->sessionModel();
    if(!model)
        return {};
    model->loadModel(in);*/
    return name;
} // namespace ModelHelper

bool saveCharacterSheet(const QString& path, const CharacterSheetModel* model) {}

bool loadCharacterSheet(const QString& path, CharacterSheetModel* model, ImageModel* imgModel, QJsonObject& root,
                        QString& qmlCode)
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

} // namespace ModelHelper
