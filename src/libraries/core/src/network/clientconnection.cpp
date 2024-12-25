/************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                               *
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *     https://rolisteam.org/                                         *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/
#include "network/clientconnection.h"

#include <QCoreApplication>
#include <QTcpSocket>

#include <QDebug>

QByteArray dataToArray(const NetworkMessageHeader& header, const char* buffer)
{
    QByteArray array;
    array.append(reinterpret_cast<const char*>(&header), sizeof(NetworkMessageHeader));
    array.append(buffer, static_cast<int>(header.dataSize));
    return array;
}

ClientConnection::ClientConnection() : m_socketTcp(new QTcpSocket(this))
{
    m_socketTcp->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    connect(m_socketTcp, &QTcpSocket::connected, this, [this]() { setConnected(true); });
    connect(m_socketTcp, &QTcpSocket::disconnected, this, [this]() { setConnected(false); });
    connect(m_socketTcp, &QTcpSocket::readyRead, this, &ClientConnection::receivingData);
    // connect(m_socketTcp, &QTcpSocket::bytesWritten, this, [](qint64 bytes) { qDebug() << bytes << "byteWrittens"; });
    connect(m_socketTcp, &QTcpSocket::stateChanged, this,
            [](const QAbstractSocket::SocketState& state)
            {
                qDebug() << "NetworkLink - state changed" << state;
                /*switch(state)
                {
                case QAbstractSocket::ClosingState:
                case QAbstractSocket::UnconnectedState:
                case QAbstractSocket::HostLookupState:
                case QAbstractSocket::ConnectingState:
                case QAbstractSocket::BoundState:
                    break;
                case QAbstractSocket::ConnectedState:
                    qDebug() << "socket state Connected";
                    break;
                default:
                    break;
                }*/
            });

    connect(m_socketTcp, &QTcpSocket::errorOccurred, this,
            [this](const QAbstractSocket::SocketError& error)
            {
                qDebug() << "NetworkLink - state changed" << error << m_socketTcp->errorString();
                if(m_socketTcp.isNull())
                    return;
                emit errorOccured(m_socketTcp->errorString());
            });

    m_receivingData= false;
    m_headerRead= 0;
}
ClientConnection::~ClientConnection()= default;

bool ClientConnection::connected() const
{
    return m_connected;
}

void ClientConnection::sendData(char* data, qint64 size)
{
    if(!m_connected)
        return;

    if(nullptr == m_socketTcp)
    {
        emit errorOccured(tr("Socket is null"));
        return;
    }
    if(!m_socketTcp->isWritable())
    {
        emit errorOccured(tr("Socket is not writable"));
        return;
    }

    // sending data
#ifdef DEBUG_MODE
    NetworkMessageHeader header;
    memcpy(reinterpret_cast<char*>(&header), data, sizeof(NetworkMessageHeader));
    qDebug() << "send Data Categorie:" << MessageDispatcher::cat2String(reinterpret_cast<NetworkMessageHeader*>(data))
             << "Action" << MessageDispatcher::act2String(reinterpret_cast<NetworkMessageHeader*>(data));

    qDebug() << "header: cat" << header.category << "act:" << header.action << "datasize:" << header.dataSize << "size"
             << size << static_cast<int>(data[0]) << "socket" << m_socketTcp;

    qDebug() << "thread current" << QThread::currentThread() << "thread socket:" << m_socketTcp->thread()
             << "this thread" << thread();
#endif

    // qDebug() << "received data:"<<size;
    auto t= m_socketTcp->write(data, size);

    if(t < 0)
    {
        emit errorOccured(tr("Transmission error :") + m_socketTcp->errorString());
    }
}

void ClientConnection::sendMessage(const NetworkMessage* msg)
{
    if(!m_connected)
        return;

    sendData(reinterpret_cast<char*>(msg->buffer()), static_cast<qint64>(msg->getSize()));
}

void ClientConnection::receivingData()
{
    if(nullptr == m_socketTcp)
    {
        return;
    }
    qint64 readData= 0;

    while(m_socketTcp->bytesAvailable())
    {
        if(!m_receivingData)
        {
            qint64 readDataSize= 0;
            char* tmp= reinterpret_cast<char*>(&m_header);
            readDataSize= m_socketTcp->read(tmp + m_headerRead, static_cast<qint64>(sizeof(NetworkMessageHeader))
                                                                    - static_cast<qint64>(m_headerRead));
            readDataSize+= static_cast<qint64>(m_headerRead);

            if((readDataSize
                != static_cast<qint64>(sizeof(NetworkMessageHeader)))) //||(m_header.category>=NetMsg::LastCategory)
            {
                m_headerRead= static_cast<quint64>(readDataSize);
                return;
            }
            else
            {
                m_headerRead= 0;
            }
            m_buffer= new char[m_header.dataSize];
            m_remainingData= m_header.dataSize;
            emit readDataReceived(m_header.dataSize, m_header.dataSize);
        }
        readData
            = m_socketTcp->read(&(m_buffer[m_header.dataSize - m_remainingData]), static_cast<qint64>(m_remainingData));
        if(readData < static_cast<qint64>(m_remainingData))
        {
            m_remainingData-= static_cast<quint32>(readData);
            m_receivingData= true;
            emit readDataReceived(m_remainingData, m_header.dataSize);
        }
        else
        {
            emit readDataReceived(0, 0);
            auto array= dataToArray(m_header, m_buffer);
            emit messageReceived(array);

            delete[] m_buffer;
            m_remainingData= 0;
            m_receivingData= false;
        }
    }
}

void ClientConnection::closeCommunicationWithServer()
{
    if(nullptr == m_socketTcp)
        return;

    m_socketTcp->disconnectFromHost();
}

void ClientConnection::connectTo(const QString& host, int port)
{
    qDebug() << "Connect To" << host << port << m_socketTcp.isNull();
    if(m_socketTcp.isNull())
        return;

    m_socketTcp->connectToHost(host, static_cast<quint16>(port));
}

void ClientConnection::setConnected(bool b)
{
    if(b == m_connected)
        return;
    m_connected= b;
    emit connectedChanged(b);
}

void ClientConnection::reset()
{
    m_inError= false;
    m_connected= false;
}
