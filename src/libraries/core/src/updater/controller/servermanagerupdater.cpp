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
#include "updater/controller/servermanagerupdater.h"

#include <QJsonDocument>
#include <QMetaObject>
#include <QSettings>

#include "network/channelmodel.h"
#include "network/serverconnection.h"
#include "worker/networkhelper.h"
#include <network/serverconnectionmanager.h>

ServerManagerUpdater::ServerManagerUpdater(ServerConnectionManager* ctrl, bool internal, QObject* parent)
    : QObject(parent), m_ctrl(ctrl), m_internal{internal}
{
    if(!m_ctrl)
        return;
    auto model= m_ctrl->channelModel();
    Q_ASSERT(model);

    auto computeModel= [this]()
    {
        auto model= m_ctrl->channelModel();
        setChannelData(helper::network::jsonObjectToByteArray(helper::network::channelModelToJSonObject(model)));

        if(!m_internal)
        {
            QSettings settings("Rolisteam", "roliserver");
            QJsonDocument doc(helper::network::channelModelToJSonObject(model));
            settings.setValue(helper::key::channel_data, doc);
        }
    };
    connect(model, &ChannelModel::modelChanged, this, computeModel);
    connect(model, &ChannelModel::dataChanged, this, computeModel);
    connect(model, &ChannelModel::rowsInserted, this, computeModel);
    connect(model, &ChannelModel::rowsMoved, this, computeModel);
    connect(model, &ChannelModel::rowsRemoved, this, computeModel);

    connect(this, &ServerManagerUpdater::channelsDataChanged, this,
            [this]()
            {
                qDebug() << "send data channel list";
                auto conns= m_ctrl->connections();
                for(auto conn : conns)
                {
                    NetworkMessageWriter* msg
                        = new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::SetChannelList);
                    msg->byteArray32(m_channelData);
                    QMetaObject::invokeMethod(conn, "sendMessage", Qt::QueuedConnection,
                                              Q_ARG(NetworkMessage*, static_cast<NetworkMessage*>(msg)),
                                              Q_ARG(bool, false));
                }
            });
}

QByteArray ServerManagerUpdater::channelsData()
{
    return m_channelData;
}

void ServerManagerUpdater::setChannelData(const QByteArray& array)
{
    qDebug() << "channel model:" << array;
    if(array == m_channelData)
        return;
    m_channelData= array;
    emit channelsDataChanged();
}
