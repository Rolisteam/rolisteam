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

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;


    int addChannel(QString name, QString password);
    bool addConnectionToChannel(QString chanId, TcpClient* client);

    void readDataJson(const QJsonObject &);
    void writeDataJson(QJsonObject &);

    void readSettings();
    void writeSettings();

    bool addConnectionToDefaultChannel(TcpClient *client);
private:
    QList<TreeItem*> m_root;
    QString m_defaultChannel;
};

#endif // CHANNELMODEL_H
