#include "tcpclient.h"
#include "channel.h"

#include <QThread>

TcpClient::TcpClient(QTcpSocket* socket,QObject *parent)
    : TreeItem(parent),m_socket(socket),m_player(new Player()),m_isAdmin(false)
{
    m_remainingData=0;
    m_headerRead = 0;

}
TcpClient::~TcpClient()
{

}
void TcpClient::connectionCheckedSlot()
{

}
void TcpClient::setSocket(QTcpSocket* socket)
{
    m_socket = socket;
    if(nullptr != m_socket)
    {

       // m_socket->setParent(this);
        connect(m_socket,&QTcpSocket::disconnected, this, &TcpClient::socketDisconnection,Qt::QueuedConnection);
        connect(m_socket,static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),this,&TcpClient::socketError,Qt::QueuedConnection);

        connect(m_socket,SIGNAL(readyRead()),this,SLOT(receivingData()));
        connect(m_socket,SIGNAL(disconnected()),this,SIGNAL(disconnected()));
        connect(m_socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(connectionError(QAbstractSocket::SocketError)));

        //connect(&m_stateMachine,SIGNAL(started()),this,SLOT(starReading()));
        connect(&m_stateMachine,SIGNAL(started()),this,SIGNAL(isReady()));
        m_incomingConnection = new QState();
        m_controlConnection = new QState();
        m_waitingAuthentificationData = new QState();
        m_authentification = new QState();
        m_wantToGoToChannel = new QState();
        m_inPlace = new QState();
        m_waitingAuthChannel = new QState();
        m_disconnected = new QState();
        m_stayInPlace = new QState();

        m_stateMachine.addState(m_incomingConnection);
        m_stateMachine.setInitialState(m_incomingConnection);

        m_stateMachine.addState(m_controlConnection);
        m_stateMachine.addState(m_waitingAuthentificationData);
        m_stateMachine.addState(m_authentification);
        m_stateMachine.addState(m_wantToGoToChannel);
        m_stateMachine.addState(m_inPlace);
        m_stateMachine.addState(m_waitingAuthChannel);
        m_stateMachine.addState(m_disconnected);
        m_stateMachine.addState(m_stayInPlace);
        m_stateMachine.start();

        connect(m_incomingConnection,&QState::activeChanged,[=](bool b){
            if(b)
            {
                m_currentState = m_incomingConnection;
            }
        });
        connect(m_controlConnection,&QState::activeChanged,[=](bool b){
            if(b)
            {
                m_currentState = m_controlConnection;
            }
        });
        connect(m_waitingAuthentificationData,&QState::activeChanged,[=](bool b){
            if(b)
            {
                m_currentState = m_waitingAuthentificationData;
            }
        });
        connect(m_authentification,&QState::activeChanged,[=](bool b){
            if(b)
            {
                m_currentState = m_authentification;
            }
        });
        connect(m_wantToGoToChannel,&QState::activeChanged,[=](bool b){
            if(b)
            {
                m_currentState = m_wantToGoToChannel;
            }
        });
        connect(m_inPlace,&QState::activeChanged,[=](bool b){
            if(b)
            {
                m_currentState = m_inPlace;
            }
        });
        connect(m_waitingAuthChannel,&QState::activeChanged,[=](bool b){
            if(b)
            {
                m_currentState = m_waitingAuthChannel;
            }
        });
        connect(m_disconnected,&QState::activeChanged,[=](bool b){
            if(b)
            {
                m_currentState = m_disconnected;
                m_socket->close();
            }
        });
        connect(m_stayInPlace,&QState::activeChanged,[=](bool b){
            if(b)
            {
                m_currentState = m_stayInPlace;
            }
        });

        connect(this,SIGNAL(connectionChecked()),this,SLOT(connectionCheckedSlot()));


        m_incomingConnection->addTransition(this, SIGNAL(hasCheck()), m_controlConnection);
        m_controlConnection->addTransition(this, SIGNAL(connectionChecked()), m_waitingAuthentificationData);
        m_controlConnection->addTransition(this, SIGNAL(forbidden()), m_disconnected);
        m_controlConnection->addTransition(this, SIGNAL(authSuccess()), m_inPlace);
        m_waitingAuthentificationData->addTransition(this, SIGNAL(authDataReceived()), m_authentification);
        m_authentification->addTransition(this, SIGNAL(authFail()), m_disconnected);
        m_authentification->addTransition(this, SIGNAL(authSuccess()), m_wantToGoToChannel);
        m_wantToGoToChannel->addTransition(this, SIGNAL(hasNoRestriction()), m_inPlace);
        m_wantToGoToChannel->addTransition(this, SIGNAL(hasRestriction()), m_waitingAuthChannel);
        m_waitingAuthChannel->addTransition(this, SIGNAL(channelAuthFail()), m_stayInPlace);
        m_waitingAuthChannel->addTransition(this, SIGNAL(channelAuthSuccess()), m_inPlace);
        m_inPlace->addTransition(this, SIGNAL(moveChannel()), m_wantToGoToChannel);

        emit socketInitiliazed();
    }

}

void TcpClient::starReading()
{
    QTcpSocket* socket = new QTcpSocket();
    qDebug() << "start reading: current thread" << QThread::currentThread() << " thread socket" << socket->thread() << " object thread" << thread();
    socket->setSocketDescriptor(getSocketHandleId());
    setSocket(socket);
}

qintptr TcpClient::getSocketHandleId() const
{
    return m_socketHandleId;
}

void TcpClient::setSocketHandleId(const qintptr &socketHandleId)
{
    m_socketHandleId = socketHandleId;
}

bool TcpClient::isAdmin() const
{
    return m_isAdmin;
}

void TcpClient::setIsAdmin(bool isAdmin)
{
    m_isAdmin = isAdmin;
}

bool TcpClient::isGM() const
{
    return m_isGM;
}

void TcpClient::setIsGM(bool isGM)
{
    if(m_isGM != isGM)
    {
        m_isGM = isGM;
        emit itemChanged();
    }

}

QString TcpClient::getPlayerId()
{
    if(nullptr != m_player)
    {
        return m_player->getUuid();
    }
    return QString();
}

void TcpClient::setInfoPlayer(NetworkMessageReader* msg)
{
    if(nullptr != m_player)
    {
        m_player->readFromMsg(*msg);

        /// @todo make it nicer.
        setName(m_player->getName());
        setId(m_player->getUuid());
    }
}

void TcpClient::fill(NetworkMessageWriter *msg)
{
    if(nullptr != m_player)
    {
        m_player->fill(*msg);
    }
}

void TcpClient::closeConnection()
{
    if(nullptr != m_socket)
    {
        m_socket->close();
    }
}

void TcpClient::addPlayerFeature(QString uuid, QString name, quint8 version)
{
    if(nullptr == m_player)
        return;

    if(m_player->getUuid() == uuid)
    {
        m_player->setFeature(name,version);
    }
}

void TcpClient::receivingData()
{
    if(nullptr==m_socket)
    {
       // qDebug() << "End of reading data socket null";
        return;
    }
    quint32 dataRead=0;


    while (m_socket->bytesAvailable())
    {
        if (!m_receivingData)
        {
            qint64 readDataSize = 0;
            char* tmp = (char *)&m_header;

            // To do only if there is enough data
            readDataSize = m_socket->read(tmp+m_headerRead, sizeof(NetworkMessageHeader)-m_headerRead);
            readDataSize+=m_headerRead;
            if(readDataSize!= sizeof(NetworkMessageHeader))
            {
              //m_headerRead=readDataSize;
              return;
            }
            else
            {
                m_headerRead=0;
            }
            m_buffer = new char[m_header.dataSize];
            m_remainingData = m_header.dataSize;
            emit readDataReceived(m_header.dataSize,m_header.dataSize);

        }
        // To do only if there is enough data
        dataRead = m_socket->read(&(m_buffer[m_header.dataSize-m_remainingData]), m_remainingData);
        m_dataReceivedTotal += dataRead;


        if (dataRead < m_remainingData)
        {
            m_remainingData -= dataRead;
            m_receivingData = true;
            emit readDataReceived(m_remainingData,m_header.dataSize);
            //m_socket->waitForReadyRead();
        }
        else
        {
            m_headerRead = 0;
            dataRead = 0;
            m_dataReceivedTotal = 0;
            emit readDataReceived(0,0);
            m_receivingData = false;
            m_remainingData = 0;
            forwardMessage();
        }
    }
}
void TcpClient::forwardMessage()
{
    QByteArray array((char*)&m_header,sizeof(NetworkMessageHeader));
    array.append(m_buffer,m_header.dataSize);
    if(m_currentState != m_disconnected)
    {
        emit dataReceived(array);
    }
}
void TcpClient::sendData(char* data, quint32 size)
{
    qint64 dataSend = m_socket->write(data,size);
    if(-1 == dataSend)
    {
        qDebug() << "error Writing data";
    }
    //qDebug() << "datasend:"<<dataSend;
}
void TcpClient::sendData(QByteArray a)
{
    qint64 dataSend = m_socket->write(a);
    if(-1 == dataSend)
    {
        qDebug() << "error Writing data";
    }
    //qDebug() << "Array datasend:"<<dataSend;
}

void TcpClient::sendMessage(NetworkMessage* msg, bool deleteMsg)
{
    if((nullptr != m_socket)&&(m_socket->isWritable()))
    {
        NetworkMessageHeader* data = msg->buffer();
        qint64 dataSend = m_socket->write((char*)data,data->dataSize+sizeof(NetworkMessageHeader));
        if(-1 == dataSend)
        {
            qDebug() << "error Writing data" << m_socket->errorString() ;
        }
    }
    if(deleteMsg)
    {
        delete msg;
    }
}
void TcpClient::connectionError(QAbstractSocket::SocketError error)
{
    emit disconnected();
}

void TcpClient::sendEvent(TcpClient::ConnectionEvent event)
{//{HasCheckEvent,NoCheckEvent,CheckedEvent,CheckFailedEvent,ForbiddenEvent,DataReceivedEvent,AuthFailEvent,AuthSuccessEvent,NoRestrictionEvent,HasRestrictionEvent,ChannelAuthSuccessEvent,ChannelAuthFailEvent};
    switch (event)
    {
    case HasCheckEvent:
        emit hasCheck();
        break;
    case NoCheckEvent:
        emit hasNoCheck();
        break;
    case CheckedEvent:
        emit connectionChecked();
        break;
    case CheckFailedEvent:
        emit checkFail();
        break;
    case ForbiddenEvent:
        emit forbidden();
        break;
    case AuthDataReceivedEvent:
        emit authDataReceived();
        break;
    case AuthFailEvent:
        emit authFail();
        break;
    case AuthSuccessEvent:
        emit authSuccess();
        break;
    case NoRestrictionEvent:
        emit hasNoRestriction();
        break;
    case HasRestrictionEvent:
        emit hasRestriction();
        break;
    case ChannelAuthSuccessEvent:
        emit channelAuthSuccess();
        break;
    case ChannelAuthFailEvent:
        emit channelAuthFail();
        break;
    case MoveChanEvent:
        emit moveChannel();
        break;
    case AdminAuthFailed:
        emit adminAuthFailed();
        m_isAdmin = false;
        break;
    case AdminAuthSuccess:
        emit adminAuthSucceed();
        m_isAdmin = true;
        break;
    default:
        qDebug() << "sendEvent Unkown event";
        break;
    }
}

Channel *TcpClient::getParentChannel() const
{
    return dynamic_cast<Channel*>(getParentItem());
}

void TcpClient::setParentChannel(Channel *parent)
{
    setParentItem(parent);
}
QTcpSocket* TcpClient::getSocket()
{
    return m_socket;
}
int TcpClient::indexOf(TreeItem *child)
{
    return -1;
}

void TcpClient::readFromJson(QJsonObject &json)
{
    m_isGM = json["gm"].toBool();
    setName(json["name"].toString());
    setId(json["id"].toString());
}

void TcpClient::writeIntoJson(QJsonObject &json)
{
    json["type"]="client";
    json["name"]=m_name;
    json["id"]=m_id;
    json["gm"]=m_isGM;
}

