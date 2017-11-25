#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QStateMachine>
#include <QState>

#include "channelmodel.h"

#include "networkmessage.h"
#include "treeitem.h"

#include "data/player.h"

class ClientManager;
class Channel;
/**
 * @brief The TcpClient class
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

    void readFromJson(QJsonObject &json);
    void writeIntoJson(QJsonObject& json);

    virtual int indexOf(TreeItem*);

    void setSocket(QTcpSocket* socket);

    bool isGM() const;
    void setIsGM(bool isGM);

    QString getPlayerId();

    void setInfoPlayer(NetworkMessageReader* msg);
    void fill(NetworkMessageWriter* msg);

    void closeConnection();

    void addPlayerFeature(QString uuid,QString  name,quint8 version);

    bool isAdmin() const;
    void setIsAdmin(bool isAdmin);

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
    void sendMessage(NetworkMessage* msg,bool deleteMsg);
    /**
     * @brief connectionError
     * @param error
     */
    void connectionError(QAbstractSocket::SocketError error);


    void sendEvent(TcpClient::ConnectionEvent);

    void connectionCheckedSlot();
    void starReading();
private:
    QTcpSocket* m_socket;
    NetworkMessageHeader m_header;
    char* m_buffer;
    int m_headerRead;
    quint32 m_dataToRead;

    QStateMachine m_stateMachine;
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

    bool m_incomingData = false;
    quint32 m_dataReceivedTotal=0;

    Player* m_player;

};

#endif // TCPCLIENT_H
