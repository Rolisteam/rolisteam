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


#ifndef NetworkLink_H
#define NetworkLink_H

#include <QtNetwork>
#include <QTcpSocket>

#include "network/networkmessage.h"
#include "mainwindow.h"

class Map;
#ifndef NULL_PLAYER
    class AudioPlayer;
#endif

/**
 * @brief The NetworkLink class
 */
class NetworkLink : public QObject
{
Q_OBJECT

public :
    /**
     * @brief NetworkLink
     * @param socket
     */
	NetworkLink(QTcpSocket *socket);
    /**
     * @brief ~NetworkLink
     */
    virtual ~NetworkLink();
    /**
     * @brief setSocket
     * @param socket
     * @param makeConnection
     */
    void setSocket(QTcpSocket* socket, bool makeConnection = true);
    /**
     * @brief disconnectAndClose
     */
    void disconnectAndClose();
    /**
     * @brief initialize
     */
    void initialize();
    /**
     * @brief insertNetWortReceiver
     * @param cat
     */
    void insertNetWortReceiver(NetWorkReceiver*,NetMsg::Category cat);
    /**
     * @brief processPlayerMessage
     * @param msg
     */
    void processPlayerMessage(NetworkMessageReader* msg);
    /**
     * @brief processSetupMessage
     * @param msg
     */
    void processSetupMessage(NetworkMessageReader* msg);



public slots :
    /**
     * @brief sendData
     * @param data
     * @param size
     * @param but
     */
    void sendData(char* data, quint32 size, NetworkLink* but = 0);


signals:
    /**
     * @brief disconnected
     * @param link
     */
    void disconnected(NetworkLink* link);
    /**
     * @brief readDataReceived
     */
    void readDataReceived(quint64,quint64);

private slots :
    void receivingData();
    void connectionError(QAbstractSocket::SocketError);
    void p_disconnect();

private :
    /**
     * @brief makeSignalConnection
     */
    void makeSignalConnection();
    /**
     * @brief postTo
     * @param obj
     */
    void postTo(QObject * obj) const;
    /**
     * @brief forwardMessage
     * @param all
     */
    void forwardMessage(NetWorkReceiver::SendType all);
    /**
     * @brief extractCharacter
     * @param map
     * @param m_buffer
     * @return
     */
    int extractCharacter(Map* map, char *m_buffer);


    QTcpSocket* m_socketTcp;
    NetworkMessageHeader m_header;
    bool m_receivingData;
    char* m_buffer;
    quint32 m_remainingData;
#ifndef NULL_PLAYER
    AudioPlayer* m_audioPlayer;
#endif
    MainWindow* m_mainWindow;
    NetworkManager* m_networkManager;
    QMap<NetMsg::Category,NetWorkReceiver*> m_receiverMap;
    int m_headerRead;
};

#endif
