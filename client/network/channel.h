#ifndef CHANNEL_H
#define CHANNEL_H

#include <QString>
#include "treeitem.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QString>

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
    Channel(QString name);
    virtual ~Channel();

    QString password() const;
    void setPassword(const QString &password);

    virtual int childCount() const;

    int indexOf(TreeItem* child);

    QString description() const;
    void setDescription(const QString &description);

    bool usersListed() const;
    void setUsersListed(bool usersListed);

    bool isLeaf() const;

    void sendToAll(NetworkMessage*, TcpClient *, bool mustBeSaved);

    void readFromJson(QJsonObject &json);
    void writeIntoJson(QJsonObject& json);
    TreeItem* getChildAt(int row);

    int addChild(TreeItem* );

    bool addChildInto(QString id, TreeItem* child);

    virtual void clear();

    void updateNewClient(TcpClient *newComer);

    bool removeChild(TcpClient* client);

    virtual void kick(QString str);
    TreeItem* getChildById(QString id);

    void fill(NetworkMessageWriter& msg);
    void read(NetworkMessageReader& msg);
private:
    QString m_password;
    QString m_description;
    bool m_usersListed;

    QList<TreeItem*> m_child;
    QList<NetworkMessage*> m_dataToSend;
};

#endif // CHANNEL_H
