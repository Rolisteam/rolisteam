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
#include "networkcontroller.h"

#include <QMetaObject>
#include <QThread>

#include "controller/gamecontroller.h"
#include "controller/playercontroller.h"
#include "model/profilemodel.h"
#include "network/clientmanager.h"
#include "network/networkmessage.h"
#include "network/receiveevent.h"
#include "network/servermanager.h"
#include "services/ipchecker.h"
#include "worker/messagehelper.h"
#include "worker/modelhelper.h"
#include "worker/playermessagehelper.h"

NetworkController::NetworkController(QObject* parent)
    : AbstractControllerInterface(parent)
    , m_clientManager(new ClientManager)
    , m_profileModel(new ProfileModel)
    , m_channelModel(new ChannelModel)
{
    qRegisterMetaType<ServerManager::ServerState>();
    Settingshelper::readConnectionProfileModel(m_profileModel.get());

    ReceiveEvent::registerNetworkReceiver(NetMsg::AdministrationCategory, this);

    connect(m_clientManager.get(), &ClientManager::connectionStateChanged, this,
            [this](ClientManager::ConnectionState state) {
                setConnected(state == ClientManager::AUTHENTIFIED);
                setConnecting(state == ClientManager::CONNECTING);
            });

    connect(m_clientManager.get(), &ClientManager::dataReceived, this, &NetworkController::downloadingData);
    connect(m_clientManager.get(), &ClientManager::messageReceived, this, &NetworkController::dispatchMessage);
    connect(m_clientManager.get(), &ClientManager::connectedToServer, this, &NetworkController::sendOffConnectionInfo);
    connect(m_clientManager.get(), &ClientManager::gameMasterStatusChanged, this, &NetworkController::isGMChanged);
    connect(m_clientManager.get(), &ClientManager::moveToAnotherChannel, this, &NetworkController::tableChanged);

    // connect(m_clientManager.get(), SIGNAL(clearData()), this, SLOT(cleanUpData()));
    // connect(m_clientManager.get(), &ClientManager::notifyUser, m_gameController.get(),
    // &GameController::addFeatureLog);
    // connect(m_clientManager.get(), &ClientManager::stopConnectionTry, this, &MainWindow::stopReconnection);
    // connect(m_clientManager.get(), &ClientManager::errorOccur, m_dialog,
    // &SelectConnectionProfileDialog::errorOccurs);
    /*connect(m_clientManager.get(), SIGNAL(connectionStateChanged(ClientManager::ConnectionState)), this,
            SLOT(updateWindowTitle()));*/
}
NetworkController::~NetworkController() {}

void NetworkController::dispatchMessage(QByteArray array)
{
    qDebug() << "dispatch array message";
    NetworkMessageReader data;
    data.setInternalData(array);
    if(ReceiveEvent::hasNetworkReceiverFor(data.category()))
    {
        QList<NetWorkReceiver*> tmpList= ReceiveEvent::getNetWorkReceiverFor(data.category());
        for(NetWorkReceiver* tmp : tmpList)
        {
            tmp->processMessage(&data);
        }
    }
}

ProfileModel* NetworkController::profileModel() const
{
    return m_profileModel.get();
}

ChannelModel* NetworkController::channelModel() const
{
    return m_channelModel.get();
}

void NetworkController::startConnection()
{
    if(hosting())
        startServer();
    startClient();
}

void NetworkController::setIsGM(bool b)
{
    if(m_isGM == b)
        return;
    m_isGM= b;
    emit isGMChanged();
}

void NetworkController::setHosting(bool b)
{
    if(m_hosting == b)
        return;
    m_hosting= b;
    emit hostingChanged();
}

void NetworkController::setPort(int port)
{
    if(m_port == port)
        return;
    m_port= port;
    emit portChanged();
}

void NetworkController::setAskForGM(bool askGM)
{
    if(m_askForGM == askGM)
        return;
    m_askForGM= askGM;
    emit askForGMChanged();
}

void NetworkController::setConnected(bool b)
{
    if(b == m_connected)
        return;
    m_connected= b;
    emit connectedChanged(m_connected);
}

void NetworkController::setConnecting(bool b)
{
    if(b == m_connecting)
        return;
    m_connecting= b;
    emit connectingChanged(m_connecting);
}

void NetworkController::setServerPassword(const QByteArray& array)
{
    if(m_serverPw == array)
        return;
    m_serverPw= array;
    emit serverPasswordChanged();
}

void NetworkController::setAdminPassword(const QByteArray& array)
{
    if(m_admindPw == array)
        return;
    m_admindPw= array;
    emit adminPasswordChanged();
}

void NetworkController::removeProfile(int pos)
{
    m_profileModel->removeProfile(pos);
}

void NetworkController::startClient()
{
    m_clientManager->connectTo(m_host, m_port);
}
void NetworkController::startServer()
{
    m_server.reset(new ServerManager());
    m_ipChecker.reset(new IpChecker());
    m_serverThread.reset(new QThread);
    m_server->moveToThread(m_serverThread.get());
    m_server->initServerManager();
    connect(m_serverThread.get(), &QThread::started, m_server.get(), &ServerManager::startListening);
    // connect(m_serverThread.get(), &QThread::finished, m_server.get(), &ServerManager::stopListening);
    connect(m_serverThread.get(), &QThread::finished, this, []() { qDebug() << "server thread terminated"; });
    connect(m_server.get(), &ServerManager::stateChanged, this, [this]() {
        switch(m_server->state())
        {
        case ServerManager::Stopped:
            m_serverThread->terminate();
            break;
        case ServerManager::Listening:
        case ServerManager::Error:
            break;
        }
    });
    connect(m_server.get(), &ServerManager::errorOccured, this,
            [this](const QString& str, LogController::LogLevel level) {
                qDebug() << str << "error on server";
                /*switch(level)
                {
                case LogController::Error:
                    m_dialog->errorOccurs(str);
                    m_gameController->addErrorLog(str);
                    break;
                case LogController::Info:
                    m_gameController->addInfoLog(str);
                    break;
                case LogController::Warning:
                    m_gameController->addWarningLog(str);
                    break;
                case LogController::Features:
                    m_gameController->addFeatureLog(str);
                    break;
                default:
                    break;
                }*/
            });
    // connect(m_server, &ServerManager::listening, this, &MainWindow::initializedClientManager,
    // Qt::QueuedConnection);

    m_server->insertField("port", m_port);
    m_server->insertField("ThreadCount", 1);
    m_server->insertField("ChannelCount", 1);
    m_server->insertField("LogLevel", 3);
    m_server->insertField("ServerPassword", serverPassword());
    m_server->insertField("TimeToRetry", 5000);
    m_server->insertField("AdminPassword", adminPassword());

    auto thread= new QThread();
    m_ipChecker->moveToThread(thread);

    connect(thread, &QThread::started, m_ipChecker.get(), &IpChecker::startCheck);
    connect(m_ipChecker.get(), &IpChecker::ipAddressChanged, this, [this, thread](const QString& str) {
        m_ipv4Address= str;
        emit ipv4Changed();
        thread->terminate();
    });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    m_serverThread->start();
    thread->start();
}

void NetworkController::stopConnecting()
{
    if(!m_connecting)
        return;

    disconnection();
}

void NetworkController::disconnection()
{
    if(!m_connected || !m_connecting)
        return;

    m_clientManager->disconnectAndClose();

    if(m_serverThread)
        m_serverThread->terminate();
}

NetWorkReceiver::SendType NetworkController::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type= NetWorkReceiver::NONE;
    switch(msg->action())
    {
    case NetMsg::EndConnectionAction:
        break;
    case NetMsg::SetChannelList:
        break;
    case NetMsg::AdminAuthSucessed:
        break;
    case NetMsg::AuthentificationFail:
        m_clientManager->setAuthentificationStatus(false);
        break;
    case NetMsg::AuthentificationSucessed:
        m_clientManager->setAuthentificationStatus(true);
        break;
    default:
        break;
    }

    return type;
}

bool NetworkController::hosting() const
{
    return m_hosting;
}

bool NetworkController::askForGM() const
{
    return m_askForGM;
}

QString NetworkController::host() const
{
    return m_host;
}

int NetworkController::port() const
{
    return m_port;
}

QString NetworkController::ipv4() const
{
    return m_ipv4Address;
}

bool NetworkController::isGM() const
{
    return m_isGM;
}

bool NetworkController::connected() const
{
    return m_connected;
}

bool NetworkController::connecting() const
{
    return m_connecting;
}

QByteArray NetworkController::serverPassword() const
{
    return m_serverPw;
}

QByteArray NetworkController::adminPassword() const
{
    return m_serverPw;
}

void NetworkController::setHost(const QString& host)
{
    if(host == m_host)
        return;
    m_host= host;
    emit hostChanged();
}

void NetworkController::setGameController(GameController* game)
{
    m_gameCtrl= game;
}

void NetworkController::sendOffConnectionInfo()
{
    if(m_gameCtrl.isNull())
        return;

    auto playerCtrl= m_gameCtrl->playerController();
    if(nullptr == playerCtrl)
        return;
    PlayerMessageHelper::sendOffConnectionInfo(playerCtrl->localPlayer(), m_serverPw);
}
void NetworkController::closeServer()
{
    if(m_server)
    {
        QMetaObject::invokeMethod(m_server.get(), &ServerManager::stopListening, Qt::QueuedConnection);
    }
}

void NetworkController::saveData()
{
    Settingshelper::writeConnectionProfileModel(m_profileModel.get());
}
