#ifndef CHANNEL_H
#define CHANNEL_H

#include <QString>
#include "treeitem.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QString>

class TcpClient;
class NetworkMessageReader;
/**
 * @brief The Channel class
 */
class Channel : public TreeItem
{
public:
    Channel();
    Channel(QString);
    virtual ~Channel();

    QString password() const;
    void setPassword(const QString &password);

    int indexOf(TreeItem* child);


    QString title() const;
    void setTitle(const QString &title);

    QString description() const;
    void setDescription(const QString &description);

    bool usersListed() const;
    void setUsersListed(bool usersListed);

    bool isLeaf() const;

    void sendToAll(NetworkMessageReader*, TcpClient *);

    void readChannel(QJsonObject &json);
    void writeChannel(QJsonObject& json);

    int addChild(TreeItem* );

private:
    QString m_password;
    QString m_title;
    QString m_description;
    bool m_usersListed;

    QList<TreeItem*> m_child;
};

#endif // CHANNEL_H
