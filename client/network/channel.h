#ifndef CHANNEL_H
#define CHANNEL_H

#include <QString>
#include "treeitem.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QString>
#include <QPointer>

#include "networkmessagewriter.h"

class TcpClient;
class NetworkMessage;
class NetworkMessageReader;
/**
 * @brief The Channel class
 */
class Channel : public TreeItem
{
    Q_OBJECT
public:
    Channel();
    explicit Channel(QString name);
    virtual ~Channel();

    QByteArray password() const;
    void setPassword(const QByteArray &password);

    virtual int childCount() const;

    int indexOf(TreeItem* child);

    QString description() const;
    void setDescription(const QString &description);

    bool usersListed() const;
    void setUsersListed(bool usersListed);

    bool isLeaf() const;

    void sendMessage(NetworkMessage*, TcpClient *, bool mustBeSaved);
    void sendToAll(NetworkMessage*, TcpClient* sender, bool deleteMsg = false);
    void sendToMany(NetworkMessage* msg, TcpClient* sender, bool deleteMsg = false);


    void readFromJson(QJsonObject &json);
    void writeIntoJson(QJsonObject& json);
    TreeItem* getChildAt(int row);

    int addChild(TreeItem* );

    bool addChildInto(QString id, TreeItem* child);

    virtual void clear();

    void updateNewClient(TcpClient *newComer);

    bool removeClient(TcpClient* client);
    bool removeChild(TreeItem*);

    bool isCurrentGm(TreeItem*);
    QString getCurrentGmId();

    virtual void kick(QString str);
    TreeItem* getChildById(QString id);
    TcpClient* getPlayerById(QString id);

    void fill(NetworkMessageWriter& msg);
    void read(NetworkMessageReader& msg);

public slots:
    void clearData();
signals:
    void memorySizeChanged(quint64 memorySize, Channel* id);
protected:
    bool hasNoClient();
    void sendOffGmStatus(TcpClient *client);
    void findNewGM();
private:
    QByteArray m_password;
    QString m_description;
    bool m_usersListed = false;

    QList<QPointer<TreeItem>> m_child;
    QList<NetworkMessage*> m_dataToSend;
    quint64 m_memorySize = 0;
    QPointer<TcpClient> m_currentGm;
};

#endif // CHANNEL_H
