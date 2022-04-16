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
#ifndef SELECTCONNPROFILECONTROLLER_H
#define SELECTCONNPROFILECONTROLLER_H

#include <QObject>
#include <QPointer>
#include <QTimer>
#include <memory>

#include "core/controller/gamecontroller.h"

struct ValidFormInfo
{
    bool validProfileName;
    bool validPlayerName;
    bool validPlayerAvatar;
    bool validCamp;
    bool validConnectionInfo;
    bool validCharacter;
};

class ProfileModel;
class ConnectionProfile;
class CharacterDataModel;
class SelectConnProfileController : public QObject
{
    Q_OBJECT
    // clang-format off
    Q_PROPERTY(ProfileModel* profileModel READ profileModel CONSTANT)
    Q_PROPERTY(CharacterDataModel* characterModel READ characterModel CONSTANT)

    Q_PROPERTY(int currentProfileIndex READ currentProfileIndex WRITE setCurrentProfileIndex NOTIFY currentProfileIndexChanged)
    Q_PROPERTY(QString profileName READ profileName WRITE setProfileName NOTIFY profileNameChanged)
    Q_PROPERTY(QString playerName READ playerName WRITE setPlayerName NOTIFY playerNameChanged)
    Q_PROPERTY(QColor playerColor READ playerColor WRITE setPlayerColor NOTIFY playerColorChanged)
    Q_PROPERTY(QByteArray playerAvatar READ playerAvatar WRITE setPlayerAvatar NOTIFY playerAvatarChanged)

    Q_PROPERTY(bool isGameMaster READ isGameMaster WRITE setIsGameMaster NOTIFY isGameMasterChanged)
    Q_PROPERTY(bool isServer READ isServer WRITE setIsServer NOTIFY isServerChanged)
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString errorMsg READ errorMsg WRITE setErrorMsg NOTIFY errorMsgChanged)
    Q_PROPERTY(QString infoMsg READ infoMsg WRITE setInfoMsg NOTIFY infoMsgChanged)
    Q_PROPERTY(QString campaignPath READ campaignPath WRITE setCampaignPath NOTIFY campaignPathChanged)

    Q_PROPERTY(ConnectionState connectionState READ connectionState WRITE setConnectionState NOTIFY connectionStateChanged)


    Q_PROPERTY(bool canConnect READ canConnect NOTIFY canConnectChanged)
    Q_PROPERTY(bool validCampaignPath READ validCampaignPath NOTIFY validCampaignPathChanged)
    Q_PROPERTY(bool validCharacter READ validCharacter NOTIFY validCharacterChanged)
    Q_PROPERTY(bool validCharacterName READ validCharacterName NOTIFY validCharacterNameChanged)
    Q_PROPERTY(bool validCharacterAvatar READ validCharacterAvatar NOTIFY validCharacterAvatarChanged)
    Q_PROPERTY(bool validCharacterColor READ validCharacterColor NOTIFY validCharacterColorChanged)

    Q_PROPERTY(bool isWorking READ isWorking NOTIFY isWorkingChanged)
    // clang-format on

public:
    enum class ConnectionState
    {
        IDLE,
        LOADING,
        LOADED,
        CONNECTING
    };
    Q_ENUM(ConnectionState)
    explicit SelectConnProfileController(ProfileModel* model, QObject* parent= nullptr);

    ProfileModel* profileModel() const;
    int currentProfileIndex() const;

    QString profileName() const;
    QString playerName() const;
    QColor playerColor() const;
    QByteArray playerAvatar() const;
    CharacterDataModel* characterModel() const;

    bool isGameMaster() const;
    bool isServer() const;
    QString address() const;
    int port() const;
    QString errorMsg() const;
    bool canConnect() const;
    QString campaignPath() const;
    ConnectionState connectionState() const;
    bool validCampaignPath() const;

    bool isWorking() const;

    bool validCharacter() const;
    bool validCharacterName() const;
    bool validCharacterAvatar() const;
    bool validCharacterColor() const;

    const QString& infoMsg() const;
    const QString password() const;

public slots:
    void setCurrentProfileIndex(int i);

    // setter
    void setProfileName(const QString& val);
    void setPlayerName(const QString& val);
    void setPlayerColor(const QColor& val);
    void setPlayerAvatar(const QByteArray& val);
    void setIsGameMaster(bool val);
    void setIsServer(bool val);
    void setAddress(const QString& val);
    void setPort(int val);
    void setErrorMsg(const QString& val);
    void setCampaignPath(const QString& val);
    void setInfoMsg(const QString& newInfoMsg);
    void setPassword(const QString& newPassword);

    // action slots
    // profile models
    void addProfile();
    void removeProfile(int idx);
    void cloneProfile(int idx);

    // action on charactermodel
    void addCharacter();
    void removeCharacter(int idx);
    void editCharacterName(int idx, const QString& data);
    void editCharacterColor(int idx, const QColor& color);
    void editCharacterAvatar(int idx, const QByteArray& array);

    // Network
    void setConnectionState(SelectConnProfileController::ConnectionState state);
    // void stopConnection();
    // void connectTo();

signals:
    void currentProfileIndexChanged();
    void profileNameChanged();
    void playerNameChanged();
    void playerColorChanged();
    void playerAvatarChanged();
    void isGameMasterChanged();
    void isServerChanged();
    void addressChanged();
    void portChanged();
    void errorMsgChanged();
    void canConnectChanged(bool);
    void validCampaignPathChanged(bool);
    void campaignPathChanged();
    void connectionStateChanged(SelectConnProfileController::ConnectionState newState,
                                SelectConnProfileController::ConnectionState oldState);

    void startConnect();
    void stopConnecting();
    void isWorkingChanged();

    void validCharacterChanged();
    void validCharacterNameChanged();
    void validCharacterAvatarChanged();
    void validCharacterColorChanged();

    void infoMsgChanged();

    void passwordChanged();

private slots:
    void setCanConnect(bool b);
    void setValidCampaignPath(bool b);
    void setWorking(bool b);

private:
    ConnectionProfile* currentProfile() const;
    void updateCanConnect();

private:
    QPointer<ProfileModel> m_profileModel;
    std::unique_ptr<CharacterDataModel> m_characterModel;
    int m_currentProfileIndex= -1;
    QString m_error;
    QString m_infoMsg;
    ConnectionState m_connectionState= ConnectionState::IDLE;
    bool m_protect= false;
    bool m_canConnect= false;
    bool m_validCampaignPath= false;

    bool m_isWorking= false;
    std::unique_ptr<QTimer> m_timer;
};

#endif // SELECTCONNPROFILECONTROLLER_H
