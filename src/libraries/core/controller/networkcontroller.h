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
#ifndef CONNECTIONCONTROLLER_H
#define CONNECTIONCONTROLLER_H

#include "controllerinterface.h"

#include <QObject>
#include <QPointer>
#include <memory>

#include "model/profilemodel.h"
#include "network/channelmodel.h"
#include "network/networkreceiver.h"

class ClientManager;
class ServerManager;
class QThread;
class HeartBeatSender;
class ProfileModel;
class QAbstractItemModel;
class ConnectionProfile;
class GameController;
class IpChecker;
class NetworkController : public AbstractControllerInterface, public NetWorkReceiver
{
    Q_OBJECT
    Q_PROPERTY(bool isGM READ isGM WRITE setIsGM NOTIFY isGMChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(bool connecting READ connecting NOTIFY connectingChanged)
    Q_PROPERTY(bool hosting READ hosting WRITE setHosting NOTIFY hostingChanged)
    Q_PROPERTY(bool askForGM READ askForGM WRITE setAskForGM NOTIFY askForGMChanged)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QByteArray adminPassword READ adminPassword WRITE setAdminPassword NOTIFY adminPasswordChanged)
    Q_PROPERTY(QByteArray serverPassword READ serverPassword WRITE setServerPassword NOTIFY serverPasswordChanged)
    Q_PROPERTY(ProfileModel* profileModel READ profileModel CONSTANT)
    Q_PROPERTY(ChannelModel* channelModel READ channelModel CONSTANT)
    Q_PROPERTY(QString ipv4 READ ipv4 NOTIFY ipv4Changed)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
public:
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

    void authentificationFail();

public slots:
    // network
    void startConnection();
    void stopConnecting();
    void stopConnection();

    void setIsGM(bool b);
    void setHosting(bool b);
    void setAskForGM(bool b);
    void setHost(const QString& host);
    void setPort(int port);
    void setServerPassword(const QByteArray& array);
    void setAdminPassword(const QByteArray& array);
    void setConnected(bool b);
    void setConnecting(bool b);
    void removeProfile(int pos);
    void closeServer();
    void saveData();

private slots:
    void sendOffConnectionInfo();

    void setLastError(const QString& error);
    void dispatchMessage(QByteArray array);

private:
    void startServer();
    void startClient();

    void stopClient();
    void stopServer();

private:
    std::unique_ptr<ClientManager> m_clientManager;
    std::unique_ptr<ServerManager> m_server;
    std::unique_ptr<QThread> m_serverThread;
    std::unique_ptr<HeartBeatSender> m_hbSender;
    std::unique_ptr<ProfileModel> m_profileModel;
    std::unique_ptr<ChannelModel> m_channelModel;
    std::unique_ptr<IpChecker> m_ipChecker;
    QPointer<GameController> m_gameCtrl;
    QHash<NetMsg::Category, NetWorkReceiver*> m_receiverMap;

    QByteArray m_serverPw;
    QByteArray m_admindPw;

    QString m_host;
    int m_port= 6660;
    QString m_ipv4Address;
    QString m_lastError;

    // Data
    bool m_isGM= true;
    bool m_hosting= false;
    bool m_askForGM= true;
    bool m_connected= false;
    bool m_connecting= false;
};

#endif // CONNECTIONCONTROLLER_H
