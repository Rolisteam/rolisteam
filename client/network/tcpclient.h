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

class Channel;
/**
 * @brief The TcpClient class reprents connection state with client in server side.
 */
class TcpClient : public TreeItem
{
    Q_OBJECT
public:
    enum ConnectionEvent {CheckSuccessEvent,CheckFailEvent,
                          ControlFailEvent,ControlSuccessEvent,
                          ServerAuthDataReceivedEvent,
                          ServerAuthFailEvent, ServerAuthSuccessEvent,
                          AdminAuthFailEvent,AdminAuthSuccessEvent,
                          ChannelAuthSuccessEvent,ChannelAuthFailEvent,ChannelAuthDataReceivedEvent,
                          ChannelChanged, MoveChanEvent};
    Q_ENUM(ConnectionEvent)
    /**
     * @brief TcpClient
     * @param socket
     * @param parent
     */
    explicit TcpClient(QTcpSocket* socket,QObject *parent = nullptr);
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

    QString getPlayerId();

    void setInfoPlayer(NetworkMessageReader* msg);
    void fill(NetworkMessageWriter* msg);



    void addPlayerFeature(QString uuid,QString  name,quint8 version);

    bool isFullyDefined();

    bool isAdmin() const;
    void setIsAdmin(bool isAdmin);
    bool isConnected()const;
    QString getIpAddress();
    QString getWantedChannel();
    QString getServerPassword() const;
    QString getAdminPassword() const;
    QString getChannelPassword() const;
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
    void checkSuccess();
    void checkFail();

    void controlFail();
    void controlSuccess();

    void serverAuthDataReceived();
    void serverAuthFail();
    void serverAuthSuccess();

    void adminAuthFailed();
    void adminAuthSucceed();

    void channelAuthFail();
    void channelAuthSuccess();

    void moveChannel();

    void clientSaysGoodBye();
    void playerStatusChanged();


    //Signal to check
    void checkServerAcceptClient(TcpClient* client);
    void checkServerPassword(TcpClient* client);
    void checkAdminPassword(TcpClient* client);
    void checkChannelPassword(TcpClient* client,QString channelId, QByteArray password);
    void channelPassword(QString channelId, QByteArray password);

    void isReady();
    void hasNoRestriction();
    void hasRestriction();
    void socketDisconnection();
    void socketError(QAbstractSocket::SocketError );
    void socketInitiliazed();
    void protocolViolation();
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
    void startReading();
    void closeConnection();

protected:
    bool isCurrentState(QState *state);
    void readAdministrationMessages(NetworkMessageReader& msg);
    void sendOffChannelChanged();
private:
    QPointer<QTcpSocket> m_socket;
    NetworkMessageHeader m_header;
    char* m_buffer= nullptr;
    int m_headerRead;
    quint64 m_remainingData;

    QStateMachine* m_stateMachine= nullptr;
    QState* m_incomingConnection= nullptr;
    QState* m_controlConnection= nullptr;
    QState* m_authentificationServer= nullptr;
    QState* m_disconnected= nullptr;

    QStateMachine* m_connected= nullptr;
    QState* m_inChannel= nullptr;
    QState* m_wantToGoToChannel= nullptr;

    QState* m_currentState = nullptr;

    bool m_isAdmin = false;
    bool m_forwardMessage = false;

    bool m_waitingData = false;
    bool m_receivingData = false;
    quint32 m_dataReceivedTotal=0;
    Player* m_player= nullptr;
    QString m_playerId;
    qintptr m_socketHandleId;
    QString m_wantedChannel;

    QString m_serverPassword;
    QString m_adminPassword;
    QString m_channelPassword;

};
Q_DECLARE_METATYPE(TcpClient::ConnectionEvent)
#endif // TCPCLIENT_H
