#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QStateMachine>
#include <QState>
#include <QPointer>

#include "channelmodel.h"

#include "networkmessage.h"
#include "treeitem.h"

#include "data/player.h"

class ClientManager;
class Channel;
/**
 * @brief The TcpClient class reprents connection state with client in server side.
 */
class TcpClient : public TreeItem
{
    Q_OBJECT
public:
    enum ConnectionEvent {HasCheckEvent,NoCheckEvent,
                          CheckedEvent,CheckFailedEvent,
                          ForbiddenEvent,AuthDataReceivedEvent,
                          AuthFailEvent, AuthSuccessEvent,
                          NoRestrictionEvent,HasRestrictionEvent,
                          ChannelAuthSuccessEvent,ChannelAuthFailEvent,
                          MoveChanEvent,AdminAuthSuccess,
                          AdminAuthFailed};
    /**
     * @brief TcpClient
     * @param socket
     * @param parent
     */
    explicit TcpClient(QTcpSocket* socket,QObject *parent = 0);
    ~TcpClient();
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
    void readFromJson(QJsonObject &json);
    void writeIntoJson(QJsonObject& json);
    virtual int indexOf(TreeItem*);
    /**
     * @brief getSocket
     * @return
     */
    QTcpSocket* getSocket();
    void setSocket(QTcpSocket* socket);
    qintptr getSocketHandleId() const;
    void setSocketHandleId(const qintptr &socketHandleId);

    bool isGM() const;
    void setIsGM(bool isGM);

    QString getPlayerId();

    void setInfoPlayer(NetworkMessageReader* msg);
    void fill(NetworkMessageWriter* msg);

    void closeConnection();

    void addPlayerFeature(QString uuid,QString  name,quint8 version);

    bool isAdmin() const;
    void setIsAdmin(bool isAdmin);
    bool isConnected()const;
signals:
    /**
     * @brief readDataReceived
     */
    void readDataReceived(quint64,quint64);
    /**
     * @brief dataReceived
     */
    void dataReceived(QByteArray);

    ///
    /// \brief connectionChecked
    void hasCheck();
    void hasNoCheck();
    void connectionChecked();
    void checkFail();
    void forbidden();
    void authDataReceived();
    void authFail();
    void authSuccess();
    void hasNoRestriction();
    void hasRestriction();
    void channelAuthFail();
    void channelAuthSuccess();
    void moveChannel();
    void adminAuthFailed();
    void adminAuthSucceed();
    void isReady();
    void socketDisconnection();
    void socketError(QAbstractSocket::SocketError );
    void socketInitiliazed();
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
     * @brief sendMessage
     * @param msg
     */
    void sendMessage(NetworkMessage* msg,bool deleteMsg);
    /**
     * @brief connectionError
     * @param error
     */
    void connectionError(QAbstractSocket::SocketError error);

    void sendEvent(TcpClient::ConnectionEvent);

    void connectionCheckedSlot();

    void startReading();
private:
    QPointer<QTcpSocket> m_socket;
    NetworkMessageHeader m_header;
    char* m_buffer;
    int m_headerRead;
    quint32 m_remainingData;

    QStateMachine* m_stateMachine;
    QState* m_incomingConnection;
    QState* m_controlConnection;
    QState* m_waitingAuthentificationData;
    QState* m_authentification;
    QState* m_wantToGoToChannel;
    QState* m_inPlace;
    QState* m_waitingAuthChannel;
    QState* m_stayInPlace;
    QState* m_disconnected;

    QState* m_currentState;

    bool m_isGM;
    bool m_isAdmin;

    bool m_receivingData = false;
    quint32 m_dataReceivedTotal=0;

    Player* m_player;

    qintptr m_socketHandleId;

};

#endif // TCPCLIENT_H
