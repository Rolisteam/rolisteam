#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

#include "networkmessage.h"


class NetworkManager;

/**
 * @brief The TcpClient class
 */
class TcpClient : public QObject
{
    Q_OBJECT
public:
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

private:
    QTcpSocket* m_socket;
    NetworkMessageHeader m_header;
    char* m_buffer;
    NetworkManager* m_networkManager;
    int m_headerRead;
    quint32 m_dataToRead;
};

#endif // TCPCLIENT_H
