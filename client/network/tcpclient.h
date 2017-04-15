#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QStateMachine>
#include <QState>

#include "channelmodel.h"

#include "networkmessage.h"

class NetworkManager;
class Channel;
/**
 * @brief The TcpClient class
 */
class TcpClient : public QObject
{
    Q_OBJECT
public:
    enum ConnectionEvent {CheckedEvent,CheckFailedEvent,ForbiddenEvent,DataReceivedEvent,AuthFailEvent,AuthSuccessEvent,NoRestrictionEvent,HasRestrictionEvent,ChannelAuthSuccessEvent,ChannelAuthFailEvent};
    /**
     * @brief TcpClient
     * @param socket
     * @param parent
     */
    explicit TcpClient(QTcpSocket* socket,QObject *parent = 0);
    /**
     * @brief getSocket
     * @return
     */
    QTcpSocket* getSocket();

    /**
     * @brief getParentChannel
     * @return
     */
    Channel *getParentChannel() const;
    /**
     * @brief setParentChannel
     * @param parent
     */
    void setParentChannel(Channel *parent);

signals:
    /**
     * @brief readDataReceived
     */
    void readDataReceived(quint64,quint64);
    /**
     * @brief dataReceived
     */
    void dataReceived(QByteArray);
    /**
     * @brief disconnected
     */
    void disconnected();

    ///
    void connectionChecked();
    void checkFail();
    void forbidden();
    void dataReceived();
    void authFail();
    void authSuccess();
    void hasNoRestriction();
    void hasRestriction();
    void channelAuthFail();
    void channelAuthSuccess();

public slots:
    /**
     * @brief receivingData
     */
    void receivingData();
    /**
     * @brief forwardMessage
     */
    void forwardMessage();
    /**
     * @brief sendData
     * @param data
     * @param size
     */
    void sendData(char* data, quint32 size);
    /**
     * @brief sendData
     * @param a
     */
    void sendData(QByteArray a);
    /**
     * @brief sendMessage
     * @param msg
     */
    void sendMessage(NetworkMessage& msg);
    /**
     * @brief connectionError
     * @param error
     */
    void connectionError(QAbstractSocket::SocketError error);


    void sendEvent(TcpClient::ConnectionEvent);

private:
    QTcpSocket* m_socket;
    NetworkMessageHeader m_header;
    char* m_buffer;
    int m_headerRead;
    quint32 m_dataToRead;
    Channel* m_parentChannel;

    QStateMachine m_stateMachine;
    QState* m_incomingConnection;
    QState* m_controlConnection;
    QState* m_waitingAuthentificationData;
    QState* m_authentification;
    QState* m_wantToGoToChannel;
    QState* m_inPlace;
    QState* m_waitingAuthChannel;

};

#endif // TCPCLIENT_H
