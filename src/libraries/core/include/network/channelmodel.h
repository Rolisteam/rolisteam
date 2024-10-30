#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

#include <QAbstractItemModel>
#include <QMimeData>
#include <QObject>

#include "network/channel.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "network/serverconnection.h"
#include "network_global.h"

class NETWORK_EXPORT ClientMimeData : public QMimeData
{
    Q_OBJECT
public:
    ClientMimeData();
    void addClient(ServerConnection* m, const QModelIndex);
    const QMap<QModelIndex, ServerConnection*>& getList() const;
    virtual bool hasFormat(const QString& mimeType) const;

private:
    QMap<QModelIndex, ServerConnection*> m_clientList;
};
/**
 * @brief The ChannelModel class
 */
class NETWORK_EXPORT ChannelModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    ChannelModel();
    ~ChannelModel();

    virtual int rowCount(const QModelIndex& parent) const;
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    virtual QModelIndex parent(const QModelIndex& child) const;
    virtual int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;

    bool setData(const QModelIndex& index, const QVariant& value, int role);

    QString addChannel(QString name, QByteArray password);
    bool addConnectionToChannel(QString chanId, ServerConnection* client);

    void readSettings();
    void writeSettings();

    bool addConnectionToDefaultChannel(ServerConnection* client);
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool hasChildren(const QModelIndex& parent) const;

    void kick(const QString&, bool isAdmin, const QString& senderId);

    TreeItem* getItemById(QString id) const;
    ServerConnection* getServerConnectionById(QString id) const;

    bool isAdmin(const QString& id) const;
    bool isGM(const QString& id, const QString& chanId) const;

    QModelIndex addChannelToIndex(Channel* channel, const QModelIndex& parent);
    bool addChannelToChannel(Channel* child, Channel* parent);
    QModelIndex channelToIndex(Channel* channel);

    void setLocalPlayerId(const QString& id);

    void removeChild(QString id);
    QStringList mimeTypes() const;
    Qt::DropActions supportedDropActions() const;
    QMimeData* mimeData(const QModelIndexList& indexes) const;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);

    void cleanUp();
    void emptyChannelMemory();
    void renameChannel(const QString& senderId, const QString& id, const QString& value);
    bool moveClient(Channel* origin, const QString& id, Channel* dest);

    const QList<QPointer<TreeItem> > &modelData();
    void resetData(QList<TreeItem*> data);
signals:
    void totalSizeChanged(quint64);
    void localPlayerGMChanged(QString id);
    void modelChanged();
    void channelNameChanged(QString id, QString name);

public slots:
    void setChannelMemorySize(Channel* chan, quint64);

protected:
    bool moveMediaItem(QList<ServerConnection*> items, const QModelIndex& parentToBe, int row,
                       QList<QModelIndex>& formerPosition);

    void appendChannel(Channel* channel);
    bool localIsGM() const;

private:
    std::pair<quint64, QString> convert(quint64 size) const;

private:
    QList<QPointer<TreeItem>> m_root;
    std::map<Channel*, quint64> m_sizeMap;
    QString m_defaultChannel;
    QString m_localPlayerId;
    bool m_admin= false;
    bool m_shield= false;
};

#endif // CHANNELMODEL_H
