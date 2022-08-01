#ifndef UPNPNAT_H
#define UPNPNAT_H

#include <QObject>
#include <string>
#include <vector>

constexpr int DefaultTimeOut{10};
constexpr int DefaultInterval{200};

class QTcpSocket;
class QUdpSocket;
class UpnpNat : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(QString localIp READ localIp NOTIFY localIpChanged)
public:
    enum class NAT_STAT
    {
        NAT_INIT= 0,
        NAT_FOUND,
        NAT_TCP_CONNECTED,
        NAT_GETDESCRIPTION,
        NAT_DESCRIPTION_FOUND,
        NAT_GETCONTROL,
        NAT_ADD,
        NAT_DEL,
        NAT_GET,
        NAT_ERROR
    };
    UpnpNat(QObject* parent= nullptr);
    void init(int time_out= DefaultTimeOut, int interval= DefaultInterval); // init
    QString lastError() const { return m_last_error; }                      // get last error
    QString localIp() const;
    NAT_STAT status() const;

public slots:
    void discovery(); // find router
    /****
     **** description: port mapping name
     **** destination_ip: internal ip address
     **** port_ex:external: external listen port
     **** destination_port: internal port
     **** protocal: TCP or UDP
     ***/
    void addPortMapping(const QString& description, const QString& destination_ip, unsigned short int port_ex,
                        unsigned short int port_in, const QString& protocol); // add port mapping

    void setDescription(const QString& xml);

signals:
    void lastErrorChanged();
    void discoveryEnd(bool b);
    void portMappingEnd(bool b);
    void statusChanged();
    void localIpChanged();

private slots:
    void setStatus(NAT_STAT status);
    void setLastError(const QString& error);
    void setLocalIp(const QString& ip);

private:
    void readDescription();  //
    bool parseDescription(); //
    void tcpConnect(const QString& host, int port, std::function<void()> onConnected,
                    std::function<void()> onReadReady);
    bool parse_mapping_info();

private:
    NAT_STAT m_status;
    int m_time_out;
    int m_interval;
    QString m_service_type;
    QString m_describe_url;
    QString m_control_url;
    QString m_base_url;
    QString m_service_describe_url;
    QString m_description_info;
    QString m_last_error;
    QString m_mapping_info;
    QString m_localIp;

    QTcpSocket* m_tcpSocket;
    QUdpSocket* m_udpSocketV4;
};

#endif
