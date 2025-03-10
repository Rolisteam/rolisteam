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
#include "controller/view_controller/selectconnprofilecontroller.h"

#include <QDebug>
#include <QtConcurrent>

#include "network/characterdatamodel.h"
#include "network/connectionprofile.h"
#include "worker/fileserializer.h"
#include "worker/utilshelper.h"

SelectConnProfileController::SelectConnProfileController(ProfileModel* model, QObject* parent)
    : QObject{parent}, m_profileModel{model}, m_characterModel{new CharacterDataModel}
{

    auto connectProfile= [this](ConnectionProfile* prof)
    {
        if(prof == nullptr)
            return;

        connect(prof, &ConnectionProfile::gmChanged, this,
                [prof, this]()
                {
                    if(!prof->isGM() && (prof->characterCount() == 0))
                    {
                        connection::CharacterData data({QUuid::createUuid().toString(),
                                                        QObject::tr("Unknown Character"), Qt::red, "",
                                                        QHash<QString, QVariant>()});
                        m_characterModel->addCharacter(data);
                    }
                });

        auto updateCharacters= [prof]()
        { prof->setCharactersValid(helper::utils::hasValidCharacter(prof->characters(), prof->isGM())); };
        connect(prof, &ConnectionProfile::characterCountChanged, this, updateCharacters);
        connect(prof, &ConnectionProfile::characterChanged, this, updateCharacters);
        connect(prof, &ConnectionProfile::gmChanged, this, updateCharacters);

        auto updatePlayerInfo= [prof]()
        {
            prof->setPlayerInfoValid(prof->playerColor().isValid() && !prof->playerName().isEmpty()
                                     && helper::utils::isSquareImage(prof->playerAvatar()));
        };
        connect(prof, &ConnectionProfile::playerAvatarChanged, this, updatePlayerInfo);
        connect(prof, &ConnectionProfile::playerColorChanged, this, updatePlayerInfo);
        connect(prof, &ConnectionProfile::playerNameChanged, this, updatePlayerInfo);

        auto updateCampaign= [prof]()
        {
            prof->setCampaignInfoValid(
                prof->isGM() ? campaign::FileSerializer::isValidCampaignDirectory(prof->campaignPath()) : true);
        };
        connect(prof, &ConnectionProfile::gmChanged, this, updateCampaign);
        connect(prof, &ConnectionProfile::campaignPathChanged, this, updateCampaign);

        updateCharacters();
        updateCampaign();
        updatePlayerInfo();
    };

    connect(m_profileModel, &ProfileModel::profileAdded, this, connectProfile);

    if(m_profileModel)
    {
        for(int i= 0; i < m_profileModel->rowCount(); ++i)
        {
            auto prof= m_profileModel->getProfile(i);
            connectProfile(prof);
        }
    }
    connect(this, &SelectConnProfileController::connectionStateChanged, this,
            [this](ConnectionState newState, ConnectionState oldState)
            {
                if(newState == ConnectionState::IDLE)
                {
                    emit stopConnecting();
                }
                if(oldState == ConnectionState::LOADING && newState == ConnectionState::LOADED)
                {
                    emit startConnect();
                }
            });

    if(m_profileModel && m_profileModel->rowCount() > 0)
        setCurrentProfileIndex(0);
}
SelectConnProfileController::~SelectConnProfileController()= default;

ProfileModel* SelectConnProfileController::profileModel() const
{
    return m_profileModel;
}

int SelectConnProfileController::currentProfileIndex() const
{
    return m_currentProfileIndex;
}

QString SelectConnProfileController::profileName() const
{
    return currentProfile() ? currentProfile()->profileTitle() : QString();
}

QString SelectConnProfileController::playerName() const
{
    return currentProfile() ? currentProfile()->playerName() : QString();
}

QColor SelectConnProfileController::playerColor() const
{
    return currentProfile() ? currentProfile()->playerColor() : QColor();
}

QByteArray SelectConnProfileController::playerAvatar() const
{
    return currentProfile() ? currentProfile()->playerAvatar() : QByteArray();
}

CharacterDataModel* SelectConnProfileController::characterModel() const
{
    return m_characterModel.get();
}
QString SelectConnProfileController::campaignPath() const
{
    return currentProfile() ? currentProfile()->campaignPath() : QString();
}

SelectConnProfileController::ConnectionState SelectConnProfileController::connectionState() const
{
    return m_connectionState;
}

bool SelectConnProfileController::validCampaignPath() const
{
    return currentProfile() ? currentProfile()->campaignInfoValid() : false;
}
bool SelectConnProfileController::isGameMaster() const
{
    return currentProfile() ? currentProfile()->isGM() : false;
}
bool SelectConnProfileController::isServer() const
{
    return currentProfile() ? currentProfile()->isServer() : false;
}
QString SelectConnProfileController::address() const
{
    return currentProfile() ? currentProfile()->address() : QString();
}
int SelectConnProfileController::port() const
{
    return currentProfile() ? currentProfile()->port() : 6660;
}
QString SelectConnProfileController::errorMsg() const
{
    return m_error;
}

bool SelectConnProfileController::canConnect() const
{
    return currentProfile() ? currentProfile()->valid() : false;
}

ConnectionProfile* SelectConnProfileController::currentProfile() const
{
    if(m_profileModel.isNull())
        return nullptr;
    return m_profileModel->getProfile(m_currentProfileIndex);
}

// setter
void SelectConnProfileController::setCurrentProfileIndex(int i)
{
    if(i == m_currentProfileIndex)
        return;
    if(currentProfile())
        disconnect(currentProfile(), 0, this, 0);
    m_currentProfileIndex= i;
    auto prof= currentProfile();
    if(prof)
    {
        connect(prof, &ConnectionProfile::titleChanged, this, &SelectConnProfileController::profileNameChanged);
        connect(prof, &ConnectionProfile::playerNameChanged, this, &SelectConnProfileController::playerNameChanged);
        connect(prof, &ConnectionProfile::playerColorChanged, this, &SelectConnProfileController::playerColorChanged);
        connect(prof, &ConnectionProfile::playerAvatarChanged, this, &SelectConnProfileController::playerAvatarChanged);
        connect(prof, &ConnectionProfile::portChanged, this, &SelectConnProfileController::portChanged);
        connect(prof, &ConnectionProfile::addressChanged, this, &SelectConnProfileController::addressChanged);
        connect(prof, &ConnectionProfile::serverChanged, this, &SelectConnProfileController::isServerChanged);
        connect(prof, &ConnectionProfile::gmChanged, this, &SelectConnProfileController::isGameMasterChanged);
        connect(prof, &ConnectionProfile::passwordChanged, this, &SelectConnProfileController::passwordChanged);
        connect(prof, &ConnectionProfile::campaignPathChanged, this, &SelectConnProfileController::campaignPathChanged);
        connect(prof, &ConnectionProfile::validChanged, this, &SelectConnProfileController::canConnectChanged);
        m_characterModel->setProfile(prof);
    }
    setConnectionState(ConnectionState::IDLE);
    emit currentProfileIndexChanged();
    emit profileNameChanged();
    emit playerNameChanged();
    emit playerColorChanged();
    emit playerAvatarChanged();
    emit portChanged();
    emit addressChanged();
    emit isServerChanged();
    emit isGameMasterChanged();
    emit passwordChanged();
    emit campaignPathChanged();
    emit canConnectChanged();

    // updateCanConnect();
}
void SelectConnProfileController::setConnectionState(ConnectionState state)
{
    if(m_connectionState == state)
        return;
    auto old= m_connectionState;
    m_connectionState= state;
    emit connectionStateChanged(m_connectionState, old);
}

void SelectConnProfileController::setProfileName(const QString& val)
{
    if(currentProfile())
        currentProfile()->setProfileTitle(val);
}
void SelectConnProfileController::setPlayerName(const QString& val)
{
    if(currentProfile())
        currentProfile()->setPlayerName(val);
}
void SelectConnProfileController::setPlayerColor(const QColor& val)
{
    if(currentProfile())
        currentProfile()->setPlayerColor(val);
}
void SelectConnProfileController::setPlayerAvatar(const QByteArray& val)
{
    if(currentProfile())
        currentProfile()->setPlayerAvatar(val);
}
void SelectConnProfileController::setIsGameMaster(bool val)
{
    if(currentProfile())
        currentProfile()->setGm(val);
}
void SelectConnProfileController::setIsServer(bool val)
{
    if(currentProfile())
        currentProfile()->setServerMode(val);
}
void SelectConnProfileController::setAddress(const QString& val)
{
    if(currentProfile())
        currentProfile()->setAddress(val);
}
void SelectConnProfileController::setPort(int val)
{
    if(currentProfile())
        currentProfile()->setPort(val);
}
void SelectConnProfileController::setErrorMsg(const QString& val)
{
    if(m_error == val)
        return;
    m_error= val;
    emit errorMsgChanged();
}

void SelectConnProfileController::setCampaignPath(const QString& val)
{
    if(currentProfile())
        currentProfile()->setCampaignPath(val);
}

void SelectConnProfileController::addCharacter()
{
    m_characterModel->insertCharacter();
}

void SelectConnProfileController::removeCharacter(int idx)
{
    m_characterModel->removeCharacter(idx);
}

void SelectConnProfileController::editCharacterName(int idx, const QString& data)
{
    m_characterModel->setName(idx, data);
}

void SelectConnProfileController::editCharacterColor(int idx, const QColor& color)
{
    m_characterModel->setColor(idx, color);
}

void SelectConnProfileController::editCharacterAvatar(int idx, const QByteArray& array)
{
    m_characterModel->setAvatar(idx, array);
}

void SelectConnProfileController::addProfile()
{
    if(!m_profileModel)
        return;
    m_profileModel->appendProfile();
}
void SelectConnProfileController::removeProfile(int idx)
{
    if(!m_profileModel)
        return;
    m_profileModel->removeProfile(idx);
}
void SelectConnProfileController::cloneProfile(int idx)
{
    if(!m_profileModel)
        return;
    m_profileModel->cloneProfile(idx);
}

bool SelectConnProfileController::validCharacter() const
{
    return currentProfile() ?
               helper::utils::hasValidCharacter(currentProfile()->characters(), currentProfile()->isGM()) :
               false;
}
bool SelectConnProfileController::validCharacterName() const
{
    if(!currentProfile())
        return false;
    auto characters= currentProfile()->characters();
    return std::any_of(std::begin(characters), std::end(characters),
                       [](const connection::CharacterData& data) { return !data.m_name.isEmpty(); });
}
bool SelectConnProfileController::validCharacterAvatar() const
{
    if(!currentProfile())
        return false;
    auto characters= currentProfile()->characters();
    return std::any_of(std::begin(characters), std::end(characters),
                       [](const connection::CharacterData& data)
                       { return helper::utils::isSquareImage(data.m_avatarData); });
}
bool SelectConnProfileController::validCharacterColor() const
{
    if(!currentProfile())
        return false;
    auto characters= currentProfile()->characters();
    return std::all_of(std::begin(characters), std::end(characters),
                       [](const connection::CharacterData& data) { return data.m_color.isValid(); });
}

const QString& SelectConnProfileController::infoMsg() const
{
    return m_infoMsg;
}

void SelectConnProfileController::setInfoMsg(const QString& newInfoMsg)
{
    if(m_infoMsg == newInfoMsg)
        return;
    m_infoMsg= newInfoMsg;
    emit infoMsgChanged();
}

const QByteArray SelectConnProfileController::password() const
{
    return currentProfile() ? currentProfile()->password() : QByteArray();
}

void SelectConnProfileController::setPassword(const QByteArray& newPassword)
{
    if(!currentProfile())
        return;

    currentProfile()->editPassword(newPassword);
}
void SelectConnProfileController::selectPlayerAvatar()
{
    emit changePlayerAvatar();
}
void SelectConnProfileController::selectCampaignPath()
{
    emit changeCampaignPath();
}

void SelectConnProfileController::selectCharacterAvatar(int i)
{
    emit changeCharacterAvatar(i);
}

void SelectConnProfileController::startConnection()
{
    setConnectionState(SelectConnProfileController::ConnectionState::LOADING);
    emit connectionStarted();
}

void SelectConnProfileController::reject()
{
    emit rejected();
}

void SelectConnProfileController::saveProfileModels()
{
    emit saveModels();
}
