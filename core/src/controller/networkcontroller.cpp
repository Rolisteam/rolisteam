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

#include "controller/gamecontroller.h"
#include "controller/playercontroller.h"
#include "model/profilemodel.h"
#include "network/clientmanager.h"
#include "network/servermanager.h"
#include "worker/messagehelper.h"
#include "worker/modelhelper.h"
#include <QThread>

NetworkController::NetworkController(QObject* parent)
    : ControllerInterface(parent), m_clientManager(new ClientManager), m_profileModel(new ProfileModel)
{
    Settingshelper::readConnectionProfileModel(m_profileModel.get());
    // connect(m_clientManager.get(), &ClientManager::notifyUser, m_gameController.get(),
    // &GameController::addFeatureLog);
    // connect(m_clientManager.get(), &ClientManager::stopConnectionTry, this, &MainWindow::stopReconnection);
    // connect(m_clientManager.get(), &ClientManager::errorOccur, m_dialog,
    // &SelectConnectionProfileDialog::errorOccurs);
    /*connect(m_clientManager.get(), SIGNAL(connectionStateChanged(ClientManager::ConnectionState)), this,
            SLOT(updateWindowTitle()));
    connect(m_clientManager.get(), SIGNAL(connectionStateChanged(ClientManager::ConnectionState)), this,
            SLOT(networkStateChanged(ClientManager::ConnectionState)));*/
    connect(m_clientManager.get(), &ClientManager::isAuthentified, this, &NetworkController::connectedChanged);
    connect(m_clientManager.get(), &ClientManager::connectedToServer, this, &NetworkController::sendOffConnectionInfo);
    // connect(m_clientManager.get(), SIGNAL(clearData()), this, SLOT(cleanUpData()));
    connect(m_clientManager.get(), &ClientManager::gameMasterStatusChanged, this, &NetworkController::isGMChanged);
    connect(m_clientManager.get(), &ClientManager::moveToAnotherChannel, this, &NetworkController::tableChanged);
}
NetworkController::~NetworkController()
{
    m_serverThread->quit();
    m_serverThread->wait(50);
}

QAbstractItemModel* NetworkController::profileModel() const
{
    return m_profileModel.get();
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
    m_serverThread.reset(new QThread);
    m_server->moveToThread(m_serverThread.get());
    m_server->initServerManager();
    connect(m_serverThread.get(), &QThread::started, m_server.get(), &ServerManager::startListening);
    connect(m_serverThread.get(), &QThread::finished, m_server.get(), &ServerManager::stopListening);
    connect(m_server.get(), &ServerManager::closed, m_serverThread.get(), &QThread::terminate);
    connect(m_server.get(), &ServerManager::finished, m_serverThread.get(), &QThread::terminate);
    connect(m_server.get(), &ServerManager::sendLog, this, [this](const QString& str, LogController::LogLevel level) {
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
    // connect(m_server, &ServerManager::listening, this, &MainWindow::initializedClientManager, Qt::QueuedConnection);

    m_server->insertField("port", m_port);
    m_server->insertField("ThreadCount", 1);
    m_server->insertField("ChannelCount", 1);
    m_server->insertField("LogLevel", 3);
    m_server->insertField("ServerPassword", serverPassword());
    m_server->insertField("TimeToRetry", 5000);
    m_server->insertField("AdminPassword", adminPassword());
    m_serverThread->start();
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

bool NetworkController::isGM() const
{
    return m_isGM;
}

bool NetworkController::connected() const
{
    return m_clientManager->isConnected();
}

QByteArray NetworkController::serverPassword() const
{
    return m_serverPw;
}

ConnectionProfile* NetworkController::getProfile(int pos) const
{
    return m_profileModel->getProfile(pos);
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

void NetworkController::appendProfile()
{
    m_profileModel->appendProfile();
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
    MessageHelper::sendOffConnectionInfo(playerCtrl->localPlayer(), m_serverPw);
}
void NetworkController::closeServer()
{
    if(m_server)
        m_server->stopListening();
}