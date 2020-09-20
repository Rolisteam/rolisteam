/************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                               *
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
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

#include <QCoreApplication>
#include <QTcpSocket>

#include "network/messagedispatcher.h"
#include "network/networklink.h"
#include "network/networkmessagewriter.h"
#include "network/receiveevent.h"

QByteArray dataToArray(const NetworkMessageHeader& header, const char* buffer)
{
    QByteArray array;
    array.append(reinterpret_cast<const char*>(&header), sizeof(NetworkMessageHeader));
    array.append(buffer, static_cast<int>(header.dataSize));
    return array;
}

NetworkLink::NetworkLink() : m_socketTcp(new QTcpSocket(this))
{
    initialize();
    m_receivingData= false;
    m_headerRead= 0;
}

bool NetworkLink::connected() const
{
    return m_connected;
}

bool NetworkLink::error() const
{
    return m_inError;
}
void NetworkLink::initialize()
{
    m_socketTcp->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    makeSignalConnection();
}

NetworkLink::~NetworkLink()= default;

void NetworkLink::makeSignalConnection()
{
    m_readyConnection= connect(m_socketTcp, &QTcpSocket::readyRead, this, &NetworkLink::receivingData);
    m_errorConnection= connect(m_socketTcp, &QTcpSocket::stateChanged, this, &NetworkLink::socketStateChanged);
}

void NetworkLink::connectionError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    if(m_socketTcp.isNull())
        return;
    setErrorMessage(m_socketTcp->errorString());
}

void NetworkLink::sendData(char* data, qint64 size)
{
    if(!m_connected)
        return;

    if(nullptr == m_socketTcp)
    {
        setErrorMessage(tr("Socket is null"));
        return;
    }
    if(!m_socketTcp->isWritable())
    {
        setErrorMessage(tr("Socket is not writable"));
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

    qDebug() << size;
    auto t= m_socketTcp->write(data, size);

    if(t < 0)
    {
        setErrorMessage(tr("Tranmission error :") + m_socketTcp->errorString());
    }
}

void NetworkLink::sendMessage(const NetworkMessage* msg)
{
    if(!m_connected)
        return;

    sendData(reinterpret_cast<char*>(msg->buffer()), static_cast<qint64>(msg->getSize()));
}

void NetworkLink::receivingData()
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
            readDataSize= m_socketTcp->read(tmp + m_headerRead,
                                            static_cast<qint64>(sizeof(NetworkMessageHeader) - m_headerRead));
            readDataSize+= m_headerRead;

            if((readDataSize != sizeof(NetworkMessageHeader))) //||(m_header.category>=NetMsg::LastCategory)
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
        readData= m_socketTcp->read(&(m_buffer[m_header.dataSize - m_remainingData]), m_remainingData);
        if(readData < m_remainingData)
        {
            m_remainingData-= readData;
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

/*void NetworkLink::processPlayerMessage(NetworkMessageReader* msg)
{
    if(NetMsg::PlayerCategory == msg->category() && NetMsg::PlayerConnectionAction == msg->action())
    {
        NetworkMessageHeader header;
        header.category= NetMsg::AdministrationCategory;
        header.action= NetMsg::EndConnectionAction;
        header.dataSize= 0;
        sendData(reinterpret_cast<char*>(&header), sizeof(NetworkMessageHeader));
    }
}*/

void NetworkLink::closeCommunicationWithServer()
{
    if(nullptr == m_socketTcp)
        return;

    setConnected(false);
    m_socketTcp->disconnectFromHost();
}
void NetworkLink::setSocket(QTcpSocket* socket)
{
    disconnect(m_readyConnection);
    disconnect(m_errorConnection);

    m_socketTcp= socket;
    initialize();
}

void NetworkLink::setError(bool b)
{
    if(m_inError == b)
        return;
    m_inError= b;
    emit errorChanged(b);
}
/*void NetworkLink::insertNetWortReceiver(NetWorkReceiver* receiver, NetMsg::Category cat)
{
    m_receiverMap.insert(cat, receiver);
}*/
void NetworkLink::connectTo(const QString& host, int port)
{
    if(m_socketTcp.isNull())
        return;

    m_socketTcp->connectToHost(host, static_cast<quint16>(port));
}

void NetworkLink::setConnected(bool b)
{
    if(b == m_connected)
        return;
    m_connected= b;
    emit connectedChanged(b);
}

void NetworkLink::setErrorMessage(const QString& erroMsg)
{
    setError(true);
    m_lastErrorMsg= erroMsg;
}

void NetworkLink::socketStateChanged(QAbstractSocket::SocketState state)
{
    switch(state)
    {
    case QAbstractSocket::ClosingState:
    case QAbstractSocket::UnconnectedState:
    case QAbstractSocket::HostLookupState:
    case QAbstractSocket::ConnectingState:
    case QAbstractSocket::BoundState:
        setConnected(false);
        break;
    case QAbstractSocket::ConnectedState:
        qDebug() << "socket state Connected";
        setConnected(true);
        break;
    default:
        break;
    }
}
/*bool NetworkLink::isOpen() const
{
    if(m_socketTcp.isNull())
        return false;

    return m_socketTcp->isOpen();
}*/

void NetworkLink::reset()
{
    m_inError= false;
    m_connected= false;
}
