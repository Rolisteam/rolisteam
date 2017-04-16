#include "tcpclient.h"
#include "channel.h"

TcpClient::TcpClient(QTcpSocket* socket,QObject *parent)
    : QObject(parent),m_socket(socket),m_parentChannel(nullptr)
{
    m_dataToRead=0;
    m_headerRead = 0;
    qInfo() << "########################################tcpClient########################################" << this;
    connect(m_socket,SIGNAL(readyRead()),this,SLOT(receivingData()));
    connect(m_socket,SIGNAL(disconnected()),this,SIGNAL(disconnected()));
    connect(m_socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(connectionError(QAbstractSocket::SocketError)));


    m_incomingConnection = new QState();
    m_controlConnection = new QState();
    m_waitingAuthentificationData = new QState();
    m_authentification = new QState();
    m_wantToGoToChannel = new QState();
    m_inPlace = new QState();
    m_waitingAuthChannel = new QState();
    m_disconnected = new QState();
    m_stayInPlace = new QState();

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
        }
    });
    connect(m_stayInPlace,&QState::activeChanged,[=](bool b){
        if(b)
        {
            m_currentState = m_stayInPlace;
        }
    });


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



    m_stateMachine.addState(m_incomingConnection);
    m_stateMachine.addState(m_controlConnection);
    m_stateMachine.addState(m_waitingAuthentificationData);
    m_stateMachine.addState(m_authentification);
    m_stateMachine.addState(m_wantToGoToChannel);
    m_stateMachine.addState(m_inPlace);
    m_stateMachine.addState(m_waitingAuthChannel);
    m_stateMachine.addState(m_disconnected);
    m_stateMachine.addState(m_stayInPlace);
    m_stateMachine.setInitialState(m_incomingConnection);

    m_stateMachine.start();
}

void TcpClient::receivingData()
{
    if(NULL==m_socket)
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
void TcpClient::sendMessage(NetworkMessage& msg)
{
    qInfo() << "send message";
    NetworkMessageHeader* data = msg.buffer();
    qint64 dataSend = m_socket->write((char*)data,data->dataSize+sizeof(NetworkMessageHeader));
    //qDebug() << "sendData MESSAGE:"<<dataSend;
    if(-1 == dataSend)
    {
        qDebug() << "error Writing data";
    }
}
void TcpClient::connectionError(QAbstractSocket::SocketError error)
{
    //qDebug() << "Error socket" <<error;
    emit disconnected();
}

void TcpClient::sendEvent(TcpClient::ConnectionEvent event)
{//{HasCheckEvent,NoCheckEvent,CheckedEvent,CheckFailedEvent,ForbiddenEvent,DataReceivedEvent,AuthFailEvent,AuthSuccessEvent,NoRestrictionEvent,HasRestrictionEvent,ChannelAuthSuccessEvent,ChannelAuthFailEvent};
    switch (event) {
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

        break;
    case AuthSuccessEvent:

        break;
    case NoRestrictionEvent:

        break;
    case HasRestrictionEvent:

        break;
    case ChannelAuthSuccessEvent:

        break;

    case ChannelAuthFailEvent:

        break;
    }
}

Channel *TcpClient::getParentChannel() const
{
    return m_parentChannel;
}

void TcpClient::setParentChannel(Channel *parent)
{
    m_parentChannel = parent;
}
QTcpSocket* TcpClient::getSocket()
{
    return m_socket;
}
