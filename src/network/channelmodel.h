#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include "tcpclient.h"

class TreeItem
{
public:
    TreeItem();

    virtual addChild();
    virtual bool isLeaf();
    virtual childCount();
    virtual void addChild(TreeItem*);

    TreeItem* getParent() const;
    void setParent(TreeItem* parent);

protected:
    QString m_name;
    TreeItem* m_parent;
};
class TcpClientItem : public TreeItem
{
    TcpClientItem(QString name,TcpClient* client);

private:
    TcpClient* m_client;
};

class Channel : public TreeItem
{
public:
    Channel();
    virtual ~Channel();

private:
    QString m_password;
    QList<TreeItem*> m_child;
};


class ChannelModel : public QAbstractItemModel
{
public:
    ChannelModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    void data(const QModelIndex &index, int role) const;

    int addChannel(QString name, QString password);
    int addConnectionToChannel(int indexChan, TcpClient* client, QString name);

private:
    QList<Channel*> m_root;
};

#endif // CHANNELMODEL_H
