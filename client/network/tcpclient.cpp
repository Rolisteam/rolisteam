#include "tcpclient.h"

//#include "networkmanager.h"

TcpClient::TcpClient(QTcpSocket* socket,QObject *parent)
    : QObject(parent),m_socket(socket)
{
    m_dataToRead=0;
    m_headerRead = 0;

    connect(m_socket,SIGNAL(readyRead()),this,SLOT(receivingData()));
    connect(m_socket,SIGNAL(disconnected()),this,SIGNAL(disconnected()));
    connect(m_socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(connectionError(QAbstractSocket::SocketError)));
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

    //qDebug()<< array.size() << "DataReceived";

    emit dataReceived(array);
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
void TcpClient::sendMessage(NetworkMessage& msg)
{
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
QTcpSocket* TcpClient::getSocket()
{
    return m_socket;
}
