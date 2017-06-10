#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

#include <QObject>
#include <QAbstractItemModel>

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "channel.h"
#include "networkreceiver.h"

class TreeItem;



/**
 * @brief The ChannelModel class
 */
class ChannelModel : public QAbstractItemModel, public NetWorkReceiver
{
    Q_OBJECT
public:
    ChannelModel();

    virtual int rowCount(const QModelIndex &parent) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role);


    int addChannel(QString name, QString password);
    bool addConnectionToChannel(QString chanId, TcpClient* client);

    void readDataJson(const QJsonObject &);
    void writeDataJson(QJsonObject &);

    void readSettings();
    void writeSettings();

    bool addConnectionToDefaultChannel(TcpClient *client);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool hasChildren(const QModelIndex &parent) const;

    void kick(QString);

    TreeItem* getItemById(QString id);

    bool isAdmin() const;
    void setAdmin(bool admin);

    QModelIndex addChannelToIndex(Channel *channel, QModelIndex &parent);
    bool addChannelToChannel(Channel* child, Channel* parent);
    QModelIndex channelToIndex(Channel *channel);

    virtual NetWorkReceiver::SendType processMessage(NetworkMessageReader *msg, NetworkLink *link);
    void removeChild(QString id);
private:
    QList<TreeItem*> m_root;
    QString m_defaultChannel;
    bool m_admin;
};

#endif // CHANNELMODEL_H
