#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include "tcpclient.h"

class TreeItem
{
public:
    TreeItem();

    virtual void addChild();
    virtual bool isLeaf() const;
    virtual int childCount() const;
    virtual int addChild(TreeItem*);
    TreeItem* getChildAt(int row);

    TreeItem* getParent() const;
    void setParent(TreeItem* parent);

class ClientMimeData : public QMimeData
{


public:
    ClientMimeData();
    void addClient(TcpClient* m,const QModelIndex);
    const QMap<QModelIndex,TcpClient*>& getList() const;
    virtual bool hasFormat(const QString & mimeType) const;
private:
    TcpClient* m_client;
};
/**
 * @brief The ChannelModel class
 */
class ChannelModel : public QAbstractItemModel, public NetWorkReceiver
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
    int addConnectionToChannel(int indexChan, TcpClient* client);

private:
    QList<TreeItem*> m_root;
    QString m_defaultChannel;
    QString m_localPlayerId;
    bool m_admin;
};

#endif // CHANNELMODEL_H
