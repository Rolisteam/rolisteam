// #include <winsock2.h>
#include <iostream>
#include <string>

#include "network/upnp/upnpnat.h"
// #include "network/upnp/xmlParser.h"

#include <QNetworkInterface>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QUrl>
#include <QXmlStreamReader>

#define MAX_BUFF_SIZE 102400

std::tuple<QString, int, QString> parseUrl(const QString& urlData)
{
    QUrl url(urlData);
    return {url.host(), url.port(), url.path()};
}

/******************************************************************
** Discovery Defines                                                 *
*******************************************************************/
// clang-format off
#define HTTPMU_HOST_ADDRESS "239.255.255.250"
#define HTTPMU_HOST_ADDRESS_V6 "FF02::1"
#define HTTPMU_HOST_PORT 1900
#define SEARCH_REQUEST_STRING "M-SEARCH * HTTP/1.1\n"            \
                              "ST:UPnP:rootdevice\n"             \
                              "MX: 3\n"                          \
                              "Man:\"ssdp:discover\"\n"          \
                              "HOST: 239.255.255.250:1900\n"     \
                                                            "\n"
#define HTTP_OK "200 OK"
#define DEFAULT_HTTP_PORT 80


/******************************************************************
** Device and Service  Defines                                                 *
*******************************************************************/

#define DEVICE_TYPE_1	"urn:schemas-upnp-org:device:InternetGatewayDevice:1"
#define DEVICE_TYPE_2	"urn:schemas-upnp-org:device:WANDevice:1"
#define DEVICE_TYPE_3	"urn:schemas-upnp-org:device:WANConnectionDevice:1"

#define SERVICE_WANIP	"urn:schemas-upnp-org:service:WANIPConnection:1"
#define SERVICE_WANPPP	"urn:schemas-upnp-org:service:WANPPPConnection:1"


/******************************************************************
** Action Defines                                                 *
*******************************************************************/
#define HTTP_HEADER_ACTION "POST %1 HTTP/1.1\r\n"                         \
                           "HOST: %2:%3\r\n"                                  \
                           "SOAPACTION:\"%4#%5\"\r\n"                           \
                           "CONTENT-TYPE: text/xml ; charset=\"utf-8\"\r\n"\
                           "Content-Length: %6 \r\n\r\n"

#define SOAP_ACTION  "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"     \
                     "<s:Envelope xmlns:s="                               \
                     "\"http://schemas.xmlsoap.org/soap/envelope/\" "     \
                     "s:encodingStyle="                                   \
                     "\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n" \
                     "<s:Body>\r\n"                                       \
                     "<u:%1 xmlns:u=\"%2\">\r\n%3"         \
                     "</u:%4>\r\n"                                        \
                     "</s:Body>\r\n"                                      \
                     "</s:Envelope>\r\n"

#define PORT_MAPPING_LEASE_TIME "0"                                //two year

#define ADD_PORT_MAPPING_PARAMS "<NewRemoteHost></NewRemoteHost>\r\n"      \
                                "<NewExternalPort>%1</NewExternalPort>\r\n"\
                                "<NewProtocol>%2</NewProtocol>\r\n"        \
                                "<NewInternalPort>%3</NewInternalPort>\r\n"\
                                "<NewInternalClient>%4</NewInternalClient>\r\n"  \
                                "<NewEnabled>1</NewEnabled>\r\n"           \
                                "<NewPortMappingDescription>%5</NewPortMappingDescription>\r\n"  \
                                "<NewLeaseDuration>"                       \
                                PORT_MAPPING_LEASE_TIME                    \
                                "</NewLeaseDuration>\r\n"

#define ACTION_ADD	 "AddPortMapping"
// clang-format on
//*********************************************************************************

UpnpNat::UpnpNat(QObject* parent) : QObject(parent) {}

UpnpNat::~UpnpNat()= default;

void UpnpNat::init(int time, int inter)
{
    m_time_out= time;
    m_interval= inter;
    setStatus(NAT_STAT::NAT_INIT);

    for(auto const& address : QNetworkInterface::allAddresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
        {
            if(m_subnet.isNull())
                setLocalIp(address.toString());
            else if(address.isInSubnet(m_subnet, m_mask))
            {
                setLocalIp(address.toString());
            }
        }
    }
}

void UpnpNat::tcpConnect(const QString& host, int port, std::function<void()> onConnected,
                         std::function<void()> onReadReady)
{
    if(m_tcpSocket)
        m_tcpSocket.release();

    m_tcpSocket.reset(new QTcpSocket(this));

    connect(m_tcpSocket.get(), &QTcpSocket::readyRead, this, [onReadReady]() { onReadReady(); });

    connect(m_tcpSocket.get(), &QTcpSocket::connected, this,
            [this, onConnected]()
            {
                setStatus(NAT_STAT::NAT_TCP_CONNECTED);
                onConnected();
            });

    connect(m_tcpSocket.get(), &QTcpSocket::errorOccurred, this,
            [this, host, port](QAbstractSocket::SocketError)
            {
                static int i= 0;
                ++i;
                setLastError(m_tcpSocket->errorString());
                (i < m_time_out) ? m_tcpSocket->connectToHost(QHostAddress(host), port) :
                                   setLastError(m_tcpSocket->errorString());
            });
    m_tcpSocket->connectToHost(QHostAddress(host), port);
}

void UpnpNat::discovery()
{
    m_udpSocketV4= new QUdpSocket(this);

    QHostAddress broadcastIpV4(HTTPMU_HOST_ADDRESS);
    // QHostAddress broadcastIpV6(HTTPMU_HOST_ADDRESS_V6);

    m_udpSocketV4->bind(QHostAddress(QHostAddress::AnyIPv4), 0);
    // m_udpSocketV6->bind(QHostAddress(QHostAddress::AnyIPv6), m_udpSocketV4->localPort());
    QByteArray datagram(SEARCH_REQUEST_STRING);

    connect(m_udpSocketV4, &QUdpSocket::readyRead, this,
            [this]()
            {
                QByteArray datagram;
                while(m_udpSocketV4->hasPendingDatagrams())
                {
                    datagram.resize(int(m_udpSocketV4->pendingDatagramSize()));
                    m_udpSocketV4->readDatagram(datagram.data(), datagram.size());
                }

                QString result(datagram);
                auto start= result.indexOf("http://");
                auto end= result.indexOf("\r", start);

                if(start < 0 || end < 0)
                {
                    setLastError(tr("Unable to read the URL in server answer"));
                    return;
                }

                m_describe_url= result.sliced(start, end - start);

                setStatus(NAT_STAT::NAT_FOUND);
                m_udpSocketV4->close();
            });

    connect(m_udpSocketV4, &QUdpSocket::errorOccurred, this,
            [this](QAbstractSocket::SocketError) { setLastError(m_tcpSocket->errorString()); });

    m_udpSocketV4->writeDatagram(datagram, broadcastIpV4, HTTPMU_HOST_PORT);
}

void UpnpNat::readDescription()
{
    auto [host, port, path]= parseUrl(m_describe_url);
    if(host.isEmpty() || port < 0 || path.isEmpty())
    {
        setLastError("Failed to parseURl: " + m_describe_url + "\n");
        return;
    }

    // connect
    QString resquest("GET %1 HTTP/1.1\r\nHost: %2:%3\r\n\r\n");
    QString http_request= resquest.arg(path, host).arg(port);

    auto connected= [this, http_request]() { m_tcpSocket->write(http_request.toLocal8Bit()); };
    auto readAll= [this]()
    {
        auto data= m_tcpSocket->readAll();
        if(m_description_info.isEmpty())
        {
            auto pos= data.indexOf("<?xml");
            if(pos >= 0)
                m_description_info= data.sliced(pos);
        }
        else
            m_description_info+= QString(data);

        if(m_description_info.contains("</root>"))
        {
            setStatus(NAT_STAT::NAT_DESCRIPTION_FOUND);
            emit discoveryEnd(parseDescription());
        }
        else
        {
            qDebug() << "end of discovery no match";
            emit discoveryEnd(false);
        }
    };
    tcpConnect(host, port, connected, readAll);
}

bool UpnpNat::parseDescription()
{
    // qDebug() << m_description_info;
    QXmlStreamReader xml(m_description_info);
    bool isType1= false;
    bool isType2= false;
    bool isType3= false;

    auto goToNextCharacter= [&xml]()
    {
        while(!xml.isCharacters() || xml.isWhitespace())
            xml.readNext();
    };

    while(!xml.atEnd())
    {
        xml.readNext();
        // qDebug() << xml.name();
        if(xml.name() == QLatin1String("URLBase"))
        {
            goToNextCharacter();
            m_base_url= xml.text().toString();
        }
        if(xml.name() == QLatin1String("deviceType"))
        {
            // QSet<QString> set{DEVICE_TYPE_1, DEVICE_TYPE_2, DEVICE_TYPE_3};
            goToNextCharacter();
            auto text= xml.text().toString();
            if(text == DEVICE_TYPE_1)
                isType1= true;
            if(text == DEVICE_TYPE_2)
                isType2= true;
            if(text == DEVICE_TYPE_3)
                isType3= true;
        }
        if(xml.name() == QLatin1String("serviceType"))
        {
            goToNextCharacter();
            auto serviceType= xml.text().toString();
            if((serviceType == QLatin1String(SERVICE_WANIP) || serviceType == QLatin1String(SERVICE_WANPPP))
               && m_service_type.isEmpty())
            {
                m_service_type= serviceType;
            }
        }
        if(xml.name() == QLatin1String("controlURL"))
        {
            goToNextCharacter();
            m_control_url= xml.text().toString();
        }
    }

    if(m_base_url.isEmpty())
    {
        auto index= m_describe_url.indexOf("/", 7);
        if(index < 0)
        {
            setLastError(tr("Fail to get base_URL from XMLNode \"URLBase\" or describe_url.\n"));
            return false;
        }
        m_base_url= m_describe_url.sliced(0, index);
    }

    if(!isType1 || !isType2 || !isType3)
    {
        setLastError(tr("Fail to find proper service type: %1 %2 %3").arg(isType1).arg(isType2).arg(isType3));
        return false;
    }

    // make the complete control_url;
    if(!m_control_url.startsWith("http://", Qt::CaseInsensitive))
        m_control_url= m_base_url + m_control_url;
    if(!m_service_describe_url.startsWith("http://", Qt::CaseInsensitive))
        m_service_describe_url= m_base_url + m_service_describe_url;

#ifndef UNIT_TEST
    qDebug() << "##############";
    qDebug() << "Service: " << m_service_type;
    qDebug() << "describe:" << m_describe_url;
    qDebug() << "Control:" << m_control_url;
    qDebug() << "Base:" << m_base_url;
    qDebug() << "service url:" << m_service_describe_url;
    qDebug() << "Description:" << m_description_info;
    qDebug() << "##############" << isType1 << isType2 << isType3;
#endif
    return true;
}

void UpnpNat::addPortMapping(const QString& description, const QString& destination_ip, unsigned short int port_ex,
                             unsigned short int port_in, const QString& protocol)
{
    auto [host, port, path]= parseUrl(m_control_url);
    if(host.isEmpty() || port < 0 || path.isEmpty())
    {
        setLastError("Fail to parseURl: " + m_describe_url + "\n");
        return;
    }

    QString action_params(ADD_PORT_MAPPING_PARAMS);

    action_params= action_params.arg(port_ex).arg(protocol).arg(port_in).arg(destination_ip).arg(description);

    QString soap_message(SOAP_ACTION);
    soap_message= soap_message.arg(ACTION_ADD).arg(m_service_type).arg(action_params).arg(ACTION_ADD);

    QString action_message(HTTP_HEADER_ACTION);
    action_message
        = action_message.arg(path).arg(host).arg(port).arg(m_service_type).arg(ACTION_ADD).arg(soap_message.size());

    QString http_request= action_message + soap_message;

    auto connected= [this, http_request]() { m_tcpSocket->write(http_request.toLocal8Bit()); };
    auto readAll= [this, description, protocol]()
    {
        auto data= m_tcpSocket->readAll();
        if(data.indexOf(HTTP_OK) < 0 && status() != NAT_STAT::NAT_ADD)
        {

            setLastError(tr("Fail to add port mapping (%1/%2) : error :%3\n")
                             .arg(description, protocol, QString::fromLatin1(data)));
            emit portMappingEnd(false);
            return;
        }
        emit portMappingEnd(true);
        setStatus(NAT_STAT::NAT_ADD);
    };

    tcpConnect(host, port, connected, readAll);
}

void UpnpNat::setStatus(NAT_STAT status)
{
    if(m_status == status)
        return;
    m_status= status;
    emit statusChanged();

    if(NAT_STAT::NAT_FOUND == m_status)
        readDescription();
}

void UpnpNat::setLocalIp(const QString& ip)
{
    if(m_localIp == ip)
        return;

    m_localIp= ip;
    emit localIpChanged();
}
QString UpnpNat::localIp() const
{
    return m_localIp;
}

UpnpNat::NAT_STAT UpnpNat::status() const
{
    return m_status;
}

void UpnpNat::setLastError(const QString& error)
{
    if(m_last_error == error)
        return;
    m_last_error= error;
    if(!m_last_error.isEmpty())
        setStatus(NAT_STAT::NAT_ERROR);
    emit lastErrorChanged();
}

QHostAddress UpnpNat::subnet() const
{
    return m_subnet;
}

void UpnpNat::setSubnet(const QHostAddress& subnet)
{
    if(m_subnet == subnet)
        return;
    m_subnet= subnet;
    emit subnetChanged();
}

int UpnpNat::mask() const
{
    return m_mask;
}
void UpnpNat::setMask(int mask)
{
    if(mask == m_mask)
        return;
    m_mask= mask;
    emit maskChanged();
}

/*bool UpnpNat::parser_description()
{
    XMLNode node= XMLNode::parseString(m_description_info.toStdString().c_str(), "root");
    if(node.isEmpty())
    {
        setLastError(tr("The device descripe XML file is not a valid XML file. Cann't find root element.\n"));
        setStatus(NAT_STAT::NAT_ERROR);
        return false;
    }

    XMLNode baseURL_node= node.getChildNode("URLBase", 0);
    if(!baseURL_node.getText())
    {
        auto index= m_describe_url.indexOf("/", 7);
        if(index < 0)
        {
            setLastError(tr("Fail to get base_URL from XMLNode \"URLBase\" or describe_url.\n"));
            setStatus(NAT_STAT::NAT_ERROR);
            return false;
        }
        m_base_url= m_describe_url.sliced(0, index);
    }
    else
        m_base_url= baseURL_node.getText();

    int num, i;
    XMLNode device_node, deviceList_node, deviceType_node;
    num= node.nChildNode("device");
    for(i= 0; i < num; i++)
    {
        device_node= node.getChildNode("device", i);
        if(device_node.isEmpty())
            break;
        deviceType_node= device_node.getChildNode("deviceType", 0);
        if(strcmp(deviceType_node.getText(), DEVICE_TYPE_1) == 0)
            break;
    }

    if(device_node.isEmpty())
    {
        setLastError("Fail to find device \"urn:schemas-upnp-org:device:InternetGatewayDevice:1 \"\n");
        setStatus(NAT_STAT::NAT_ERROR);
        return false;
    }

    deviceList_node= device_node.getChildNode("deviceList", 0);
    if(deviceList_node.isEmpty())
    {
        setLastError(" Fail to find deviceList of device \"urn:schemas-upnp-org:device:InternetGatewayDevice:1 \"\n");
        setStatus(NAT_STAT::NAT_ERROR);
        return false;
    }

    // get urn:schemas-upnp-org:device:WANDevice:1 and it's devicelist
    num= deviceList_node.nChildNode("device");
    for(i= 0; i < num; i++)
    {
        device_node= deviceList_node.getChildNode("device", i);
        if(device_node.isEmpty())
            break;
        deviceType_node= device_node.getChildNode("deviceType", 0);
        if(strcmp(deviceType_node.getText(), DEVICE_TYPE_2) == 0)
            break;
    }

    if(device_node.isEmpty())
    {
        setLastError(tr("Fail to find device \"urn:schemas-upnp-org:device:WANDevice:1 \"\n"));
        setStatus(NAT_STAT::NAT_ERROR);
        return false;
    }

    deviceList_node= device_node.getChildNode("deviceList", 0);
    if(deviceList_node.isEmpty())
    {
        setLastError(tr(" Fail to find deviceList of device \"urn:schemas-upnp-org:device:WANDevice:1 \"\n"));
        setStatus(NAT_STAT::NAT_ERROR);
        return false;
    }

    // get urn:schemas-upnp-org:device:WANConnectionDevice:1 and it's servicelist
    num= deviceList_node.nChildNode("device");
    for(i= 0; i < num; i++)
    {
        device_node= deviceList_node.getChildNode("device", i);
        if(device_node.isEmpty())
            break;
        deviceType_node= device_node.getChildNode("deviceType", 0);
        if(strcmp(deviceType_node.getText(), DEVICE_TYPE_3) == 0)
            break;
    }
    if(device_node.isEmpty())
    {
        setLastError("Fail to find device \"urn:schemas-upnp-org:device:WANConnectionDevice:1\"\n");
        setStatus(NAT_STAT::NAT_ERROR);
        return false;
    }

    XMLNode serviceList_node, service_node, serviceType_node;
    serviceList_node= device_node.getChildNode("serviceList", 0);
    if(serviceList_node.isEmpty())
    {
        setLastError(" Fail to find serviceList of device \"urn:schemas-upnp-org:device:WANDevice:1 \"\n");
        setStatus(NAT_STAT::NAT_ERROR);
        return false;
    }

    num= serviceList_node.nChildNode("service");
    const char* serviceType;
    bool is_found= false;
    for(i= 0; i < num; i++)
    {
        service_node= serviceList_node.getChildNode("service", i);
        if(service_node.isEmpty())
            break;
        serviceType_node= service_node.getChildNode("serviceType");
        if(serviceType_node.isEmpty())
            continue;
        serviceType= serviceType_node.getText();
        if(strcmp(serviceType, SERVICE_WANIP) == 0 || strcmp(serviceType, SERVICE_WANPPP) == 0)
        {
            is_found= true;
            break;
        }
    }

    if(!is_found)
    {
        setLastError("can't find  \" SERVICE_WANIP \" or \" SERVICE_WANPPP \" service.\n");
        setStatus(NAT_STAT::NAT_ERROR);
        return false;
    }

    m_service_type= serviceType;

    XMLNode controlURL_node= service_node.getChildNode("controlURL");
    m_control_url= controlURL_node.getText();

    // make the complete control_url;
    if(m_control_url.indexOf("http://") < 0 && m_control_url.indexOf("HTTP://") < 0)
        m_control_url= m_base_url + m_control_url;
    if(m_service_describe_url.indexOf("http://") < 0 && m_service_describe_url.indexOf("HTTP://") < 0)
        m_service_describe_url= m_base_url + m_service_describe_url;

    qDebug() << "-------------";
    qDebug() << "Service: " << m_service_type;
    qDebug() << "describe:" << m_describe_url;
    qDebug() << "Control:" << m_control_url;
    qDebug() << "Base:" << m_base_url;
    qDebug() << "service url:" << m_service_describe_url;
    qDebug() << "Description:" << m_description_info;
    qDebug() << "-------------";
    m_tcpSocket->close();
    setStatus(NAT_STAT::NAT_GETCONTROL);
    return true;
}*/
