#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QMimeData>

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "channel.h"
#include "networkreceiver.h"

class TreeItem;


class ClientMimeData : public QMimeData
{
Q_OBJECT
public:
    ClientMimeData();
    void addClient(TcpClient* m,const QModelIndex);
    const QMap<QModelIndex,TcpClient*>& getList() const;
    virtual bool hasFormat(const QString & mimeType) const;
private:
    QMap<QModelIndex,TcpClient*> m_clientList;
};
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


    int addChannel(QString name, QByteArray password);
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

    void setLocalPlayerId(const QString &id);

    virtual NetWorkReceiver::SendType processMessage(NetworkMessageReader *msg);
    void removeChild(QString id);
    QStringList mimeTypes() const;
    Qt::DropActions supportedDropActions() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);


    void cleanUp();
    void emptyChannelMemory();
signals:
    void totalSizeChanged(quint64);

public slots:
    void setChannelMemorySize(Channel* chan, quint64);
protected:
    bool moveMediaItem(QList<TcpClient *> items, const QModelIndex &parentToBe, int row, QList<QModelIndex> &formerPosition);
private:
    QList<TreeItem*> m_root;
    std::map<Channel*,quint64> m_sizeMap;
    QString m_defaultChannel;
    QString m_localPlayerId;
    bool m_admin;
    bool m_shield = false;
};

#endif // CHANNELMODEL_H
