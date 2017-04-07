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

    QString getName() const;
    void setName(const QString &name);

    virtual int indexOf(TreeItem*) = 0;
    int rowInParent();

protected:
    QString m_name;
    TreeItem* m_parent;
};
class TcpClientItem : public TreeItem
{


public:
    TcpClientItem(QString name,TcpClient* client);

    TcpClient* client() const;
    void setClient(TcpClient *client);

    virtual int indexOf(TreeItem* child);

private:
    TcpClient* m_client;
};

class Channel : public TreeItem
{
public:
    Channel(QString);
    virtual ~Channel();

    QString password() const;
    void setPassword(const QString &password);

    int indexOf(TreeItem* child);
private:
    QString m_password;
    QList<TreeItem*> m_child;
};


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
    int addConnectionToChannel(int indexChan, TcpClient* client);

private:
    QList<TreeItem*> m_root;
};

#endif // CHANNELMODEL_H
