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

#include "controller/networkcontroller.h"
#include "network/characterdatamodel.h"
#include "network/connectionprofile.h"

#include "worker/fileserializer.h"
#include "worker/utilshelper.h"

constexpr int timeInterval{100};

SelectConnProfileController::SelectConnProfileController(ProfileModel* model, QObject* parent)
    : QObject{parent}, m_profileModel{model}, m_characterModel{new CharacterDataModel}, m_timer{new QTimer}
{
    m_timer->setInterval(timeInterval);
    connect(this, &SelectConnProfileController::connectionStateChanged, this,
            [this](ConnectionState newState, ConnectionState oldState) {
                if(newState == ConnectionState::IDLE)
                {
                    emit stopConnecting();
                }
                if(oldState == ConnectionState::LOADING && newState == ConnectionState::LOADED)
                {
                    emit startConnect();
                }
            });

    connect(m_timer.get(), &QTimer::timeout, this, &SelectConnProfileController::updateCanConnect);

    if(m_profileModel && m_profileModel->rowCount() > 0)
        setCurrentProfileIndex(0);

    auto func= [this]() { m_timer->start(); };

    connect(this, &SelectConnProfileController::profileNameChanged, this, func);
    connect(this, &SelectConnProfileController::playerNameChanged, this, func);
    // connect(this, &SelectConnProfileController::playerColorChanged, this, func);
    connect(this, &SelectConnProfileController::playerAvatarChanged, this, func);
    connect(this, &SelectConnProfileController::portChanged, this, func);
    connect(this, &SelectConnProfileController::addressChanged, this, func);
    connect(this, &SelectConnProfileController::isServerChanged, this, func);
    connect(this, &SelectConnProfileController::isGameMasterChanged, this, func);
    connect(this, &SelectConnProfileController::campaignPathChanged, this, func);
    connect(m_characterModel.get(), &CharacterDataModel::rowsInserted, this, func);
    connect(m_characterModel.get(), &CharacterDataModel::rowsRemoved, this, func);
    connect(m_characterModel.get(), &CharacterDataModel::modelReset, this, func);
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
    return m_validCampaignPath;
}

bool SelectConnProfileController::isWorking() const
{
    return m_isWorking;
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
    return m_canConnect;
}

void SelectConnProfileController::setCanConnect(bool b)
{
    if(b == m_canConnect)
        return;
    m_canConnect= b;
    emit canConnectChanged(m_canConnect);
}

void SelectConnProfileController::setValidCampaignPath(bool b)
{
    if(b == m_validCampaignPath)
        return;
    m_validCampaignPath= b;
    emit validCampaignPathChanged(b);
}

void SelectConnProfileController::setWorking(bool b)
{
    if(b == m_isWorking)
        return;
    m_isWorking= b;
    emit isWorkingChanged();
}

void SelectConnProfileController::updateCanConnect()
{
    m_timer->stop();
    if(isWorking())
        return;
    setCanConnect(false);

    setWorking(true);
    auto port= this->port();
    auto isServer= this->isServer();
    auto isGm= this->isGameMaster();
    auto profile= this->currentProfile();
    auto address= this->address();
    auto campaignPath= this->campaignPath();
    auto profileName= this->profileName();
    auto playerName= this->playerName();
    auto playerAvatar= this->playerAvatar();
    helper::utils::setContinuation<ValidFormInfo>(
        QtConcurrent::run([port, isServer, isGm, profile, address, campaignPath, profileName, playerName,
                           playerAvatar]() -> ValidFormInfo {
            if(!profile)
                return {};
            bool validConnectionInfo= (port != 0 && isServer ? isServer : !address.isEmpty());
            bool validCamp= isGm ? campaign::FileSerializer::isValidCampaignDirectory(campaignPath) : true;
            bool validCharacter= helper::utils::hasValidCharacter(profile->characters(), isGm);
            return {
                !profileName.isEmpty(), !playerName.isEmpty(), helper::utils::isSquareImage(playerAvatar), validCamp,
                validConnectionInfo,    validCharacter};
        }),
        this, [this](const ValidFormInfo& result) {
            setCanConnect(result.validCamp && result.validConnectionInfo && result.validPlayerAvatar
                          && result.validCharacter && result.validPlayerName && result.validProfileName);
            qDebug() << "validForm" << result.validCamp << result.validCharacter << result.validConnectionInfo
                     << result.validPlayerAvatar << result.validPlayerName << result.validProfileName;
            setValidCampaignPath(result.validCamp);
            setWorking(false);
        });
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
    updateCanConnect();
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
    return std::any_of(std::begin(characters), std::end(characters), [](const connection::CharacterData& data) {
        return helper::utils::isSquareImage(data.m_avatarData);
    });
}
bool SelectConnProfileController::validCharacterColor() const
{
    if(!currentProfile())
        return false;
    auto characters= currentProfile()->characters();
    return std::any_of(std::begin(characters), std::end(characters),
                       [](const connection::CharacterData& data) { return !data.m_color.isValid(); });
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

const QString SelectConnProfileController::password() const
{
    return currentProfile() ? currentProfile()->password() : QString();
}

void SelectConnProfileController::setPassword(const QString& newPassword)
{
    if(!currentProfile())
        return;

    currentProfile()->editPassword(newPassword);
}
