﻿/***************************************************************************
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
#ifndef CONNECTIONCONTROLLER_H
#define CONNECTIONCONTROLLER_H

#include "controllerinterface.h"

#include <QHash>
#include <QObject>
#include <QPointer>
#include <memory>

#include "network/networkmessage.h"
#include "network/networkreceiver.h"
#include <core_global.h>

#include "model/profilemodel.h"
#include "network/channelmodel.h"

class ClientManager;
class RServer;
class QThread;
class HeartBeatSender;
class QAbstractItemModel;
class GameController;
class IpChecker;
class CountDownObject;

/**
 * @brief The NetworkController class is the controller on client side to use network API
 */
class CORE_EXPORT NetworkController : public AbstractControllerInterface, public NetWorkReceiver
{
    Q_OBJECT
    Q_PROPERTY(bool isGM READ isGM NOTIFY isGMChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(bool connecting READ connecting NOTIFY connectingChanged)
    Q_PROPERTY(bool hosting READ hosting NOTIFY hostingChanged)
    Q_PROPERTY(bool askForGM READ askForGM NOTIFY askForGMChanged)
    Q_PROPERTY(QString host READ host NOTIFY hostChanged)
    Q_PROPERTY(int port READ port NOTIFY portChanged)
    Q_PROPERTY(QByteArray adminPassword READ adminPassword WRITE setAdminPassword NOTIFY adminPasswordChanged)
    Q_PROPERTY(QByteArray serverPassword READ serverPassword NOTIFY serverPasswordChanged)
    Q_PROPERTY(ProfileModel* profileModel READ profileModel CONSTANT)
    Q_PROPERTY(ChannelModel* channelModel READ channelModel CONSTANT)
    Q_PROPERTY(QString ipv4 READ ipv4 NOTIFY ipv4Changed)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(int selectedProfileIndex READ selectedProfileIndex WRITE setSelectedProfileIndex NOTIFY
                   selectedProfileIndexChanged FINAL)
    Q_PROPERTY(NetworkController::Groups groups READ groups WRITE setGroups NOTIFY groupsChanged FINAL)
public:
    enum Group
    {
        VIEWER= 0x1,
        GAMEMASTER= 0x1,
        ADMIN= 0x2
    };
    Q_DECLARE_FLAGS(Groups, Group)

    explicit NetworkController(QObject* parent= nullptr);
    ~NetworkController() override;
    bool isGM() const;
    bool connected() const;
    bool connecting() const;
    bool hosting() const;
    bool askForGM() const;
    QString host() const;
    int port() const;
    QString ipv4() const;
    QString lastError() const;

    ProfileModel* profileModel() const;
    ChannelModel* channelModel() const;

    QByteArray adminPassword() const;
    QByteArray serverPassword() const;

    void setGameController(GameController* game) override;

    void insertNetWortReceiver(NetWorkReceiver*, NetMsg::Category cat);
    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;

    int selectedProfileIndex() const;
    void setSelectedProfileIndex(int newSelectedProfileIndex);
    ConnectionProfile* currentProfile() const;

    bool isAdmin() const;
    NetworkController::Groups groups() const;
    void setGroups(NetworkController::Groups group);

signals:
    void isGMChanged(bool);
    void connectedChanged(bool);
    void connectingChanged(bool);
    void hostingChanged();
    void askForGMChanged();
    void hostChanged();
    void portChanged();
    void serverPasswordChanged();
    void adminPasswordChanged();
    void ipv4Changed();
    void downloadingData(quint64 readData, quint64 size);
    void tableChanged();
    void lastErrorChanged(const QString& error);
    void infoMessage(const QString& msg);
    void authentificationFail();
    void selectedProfileIndexChanged();
    void groupsChanged();

public slots:
    // network
    void startConnection();
    void stopConnecting();
    void stopConnection();
    void setAdminPassword(const QByteArray& array);
    void setConnected(bool b);
    void setConnecting(bool b);
    void removeProfile(int pos);
    void closeServer();
    void saveData();

    void sendOffLoginAdmin(const QString& password);
    void lockChannel(const QString& uuid, NetMsg::Action action);
    void banUser(const QString& uuid, const QString& playerId);
    void kickUser(const QString& uuid, const QString& playerId);
    void addChannel(const QString& parentId);
    void resetChannel(const QString& channelId);
    void deleteChannel(const QString& channelId);
    void definePasswordOnChannel(const QString& channelId, const QByteArray& password);

private slots:
    void sendOffConnectionInfo();

    void setLastError(const QString& error);
    void dispatchMessage(QByteArray array);

private:
    void startServer();
    void startClient();
    void stopClient();

private:
    std::unique_ptr<ClientManager> m_clientManager;
    std::unique_ptr<RServer> m_server;
    std::unique_ptr<QThread> m_serverThread;
    std::unique_ptr<ProfileModel> m_profileModel;
    std::unique_ptr<ChannelModel> m_channelModel;
    std::unique_ptr<IpChecker> m_ipChecker;
    std::unique_ptr<CountDownObject> m_countDown;
    QPointer<GameController> m_gameCtrl;
    QHash<NetMsg::Category, NetWorkReceiver*> m_receiverMap;

    QMap<QString, QVariant> m_serverParameters;

    // QByteArray m_serverPw;
    QByteArray m_admindPw;
    QString m_ipv4Address;
    QString m_lastError;

    // Data
    bool m_connected= false;
    bool m_connecting= false;

    int m_selectedProfileIndex{0};
    Groups m_currentGroups{Group::VIEWER};
};

Q_DECLARE_OPERATORS_FOR_FLAGS(NetworkController::Groups)

#endif // CONNECTIONCONTROLLER_H
