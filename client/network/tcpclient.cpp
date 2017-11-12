#include "tcpclient.h"
#include "channel.h"

TcpClient::TcpClient(QTcpSocket* socket,QObject *parent)
    : TreeItem(parent),m_socket(socket),m_player(new Player()),m_isAdmin(false)
{
    m_dataToRead=0;
    m_headerRead = 0;

}
void TcpClient::connectionCheckedSlot()
{
    qDebug() << "connection checked";
}
void TcpClient::setSocket(QTcpSocket* socket)
{
    m_socket = socket;
    if(nullptr != m_socket)
    {
       // m_socket->setParent(this);
        connect(m_socket,SIGNAL(readyRead()),this,SLOT(receivingData()));
        connect(m_socket,SIGNAL(disconnected()),this,SIGNAL(disconnected()));
        connect(m_socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(connectionError(QAbstractSocket::SocketError)));


        connect(&m_stateMachine,SIGNAL(started()),this,SLOT(starReading()));
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
                //qDebug() << "Incoming State";
                m_currentState = m_incomingConnection;
            }
        });
        connect(m_controlConnection,&QState::activeChanged,[=](bool b){
            if(b)
            {
               // qDebug() << "Control State";
                m_currentState = m_controlConnection;
            }
        });
        connect(m_waitingAuthentificationData,&QState::activeChanged,[=](bool b){
            if(b)
            {
                //qDebug() << "WaitingAuthData State";
                m_currentState = m_waitingAuthentificationData;
            }
        });
        connect(m_authentification,&QState::activeChanged,[=](bool b){
            if(b)
            {
                //qDebug() << "Authification State";
                m_currentState = m_authentification;
            }
        });
        connect(m_wantToGoToChannel,&QState::activeChanged,[=](bool b){
            if(b)
            {
                //qDebug() << "WantToGoToChannel State";
                m_currentState = m_wantToGoToChannel;
            }
        });
        connect(m_inPlace,&QState::activeChanged,[=](bool b){
            if(b)
            {
                //qDebug() << "InPlace State";
                m_currentState = m_inPlace;

            }
        });
        connect(m_waitingAuthChannel,&QState::activeChanged,[=](bool b){
            if(b)
            {
               // qDebug() << "WaitingAuthChannel State";
                m_currentState = m_waitingAuthChannel;
            }
        });
        connect(m_disconnected,&QState::activeChanged,[=](bool b){
            if(b)
            {
               // qDebug() << "disconnected State";
                m_currentState = m_disconnected;
                m_socket->close();
            }
        });
        connect(m_stayInPlace,&QState::activeChanged,[=](bool b){
            if(b)
            {
               // qDebug() << "StayInPlace State";
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
    }

}

void TcpClient::starReading()
{
   /* qInfo() << m_stateMachine.isRunning();
    qInfo() << "########################################tcpClient########################################" << this;*/

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
    qInfo() << "########################################tcpClient########################################\n Receiving Data";
    if(nullptr==m_socket)
    {
        return;
    }
    quint32 dataReceived=0;
    bool incomingData = false;
    while (m_socket->bytesAvailable())
    {
        if (!incomingData)
        {
            qint64 size = 0;
            char* tmp = (char *)&m_header;
            size = m_socket->read(tmp+m_headerRead, sizeof(NetworkMessageHeader)-m_headerRead);
            size+=m_headerRead;
            if(size!= sizeof(NetworkMessageHeader))
            {
              //m_headerRead=size;
              return;
            }
            else
            {
                m_headerRead=0;
            }
            //qDebug() << "Allocation size"<<m_header.dataSize << "action" <<m_header.action << "cat" <<m_header.category;
            m_buffer = new char[m_header.dataSize];
            m_dataToRead = m_header.dataSize;
            emit readDataReceived(m_header.dataSize,m_header.dataSize);

        }
        dataReceived = m_socket->read(&(m_buffer[m_header.dataSize-m_dataToRead]), m_dataToRead);
        if (dataReceived < m_dataToRead)
        {
            m_dataToRead-=dataReceived;
            incomingData = true;
            emit readDataReceived(m_dataToRead,m_header.dataSize);
        }
        else
        {
            emit readDataReceived(0,0);
            forwardMessage();
        }
    }

}
void TcpClient::forwardMessage()
{
   // qDebug() <<"[forwardMessage]" <<m_header.dataSize << m_header.action << m_header.category;
    QByteArray array((char*)&m_header,sizeof(NetworkMessageHeader));
    array.append(m_buffer,m_header.dataSize);
    if(m_currentState != m_disconnected)
    {
        emit dataReceived(array);
    }
}
void TcpClient::sendData(char* data, quint32 size)
{
    qInfo() << "send data (char*)";
    qint64 dataSend = m_socket->write(data,size);
    if(-1 == dataSend)
    {
        qDebug() << "error Writing data";
    }
    //qDebug() << "datasend:"<<dataSend;
}
void TcpClient::sendData(QByteArray a)
{
    qInfo() << "send data";
    qint64 dataSend = m_socket->write(a);
    if(-1 == dataSend)
    {
        qDebug() << "error Writing data";
    }
    //qDebug() << "Array datasend:"<<dataSend;
}
#include <QThread>
void TcpClient::sendMessage(NetworkMessage* msg, bool deleteMsg)
{
   // qDebug() << "send message" << msg << deleteMsg;
    if((nullptr != m_socket)&&(m_socket->isWritable()))
    {
        NetworkMessageHeader* data = msg->buffer();
        qint64 dataSend = m_socket->write((char*)data,data->dataSize+sizeof(NetworkMessageHeader));
        //qDebug() << "sendData MESSAGE:"<<dataSend;
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
    //qDebug() << "Error socket" <<error;
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
    case DataReceivedEvent:
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

