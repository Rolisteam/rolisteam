#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

#include <QObject>
#include <QAbstractItemModel>

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "channel.h"


class TreeItem;



/**
 * @brief The ChannelModel class
 */
class ChannelModel : public QAbstractItemModel
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

    bool isAdmin() const;
    void setAdmin(bool admin);

    QModelIndex addChannelToChannel(Channel *channel, QModelIndex &parent);
private:
    QList<TreeItem*> m_root;
    QString m_defaultChannel;
    bool m_admin;
};

#endif // CHANNELMODEL_H
