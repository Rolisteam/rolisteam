/*************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                               *
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
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

#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <QPointer>
#include <QTcpSocket>

#include "connectionprofile.h"
#include "network/networkmessage.h"
#include "network_global.h"
#include "networkreceiver.h"

/**
 * @brief The NetworkLink [Client side] class.
 * Send data from client to server
 * Read data from server to client
 * manage the socket to server
 */
class NETWORK_EXPORT ClientConnection : public QObject, public MessageSenderInterface
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
public:
    ClientConnection();
    virtual ~ClientConnection() override;

    bool connected() const;
    QString lastErrorMessage() const;

public slots:
    void reset();
    void connectTo(const QString& host, int port);
    void closeCommunicationWithServer();

    void sendData(char* data, qint64 size);
    void sendMessage(const NetworkMessage* msg) override;

signals:
    void readDataReceived(quint64, quint64);
    void messageReceived(const QByteArray);
    void connectedChanged(bool);
    void errorOccured(QString error);

private slots:
    void setConnected(bool);
    void receivingData();

private:
    void makeSignalConnection();

private:
    QPointer<QTcpSocket> m_socketTcp;
    bool m_inError= false;
    bool m_connected= false;
    QString m_lastErrorMsg;

    NetworkMessageHeader m_header;
    bool m_receivingData;
    char* m_buffer= nullptr;
    quint32 m_remainingData;
    quint64 m_headerRead;
};

#endif // CLIENTCONNECTION_H
