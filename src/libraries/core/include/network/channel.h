/*************************************************************************
 *   Copyright (C) 2018 by Renaud Guezennec                              *
 *                                                                       *
 *   https://rolisteam.org/                                           *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/
#ifndef CHANNEL_H
#define CHANNEL_H

#include "treeitem.h"
#include <QString>

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QPointer>
#include <QString>

#include "network/tcpclient.h"
#include "network_global.h"
#include "networkmessagewriter.h"

class NetworkMessage;
class NetworkMessageReader;
/**
 * @brief The Channel class
 */
class NETWORK_EXPORT Channel : public TreeItem
{
    Q_OBJECT
    Q_PROPERTY(quint64 memorySize READ memorySize WRITE setMemorySize NOTIFY memorySizeChanged)
    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY lockedChanged)
    Q_PROPERTY(TcpClient* currentGM READ currentGM WRITE setCurrentGM NOTIFY currentGMChanged)
public:
    Channel();
    explicit Channel(QString name);
    virtual ~Channel();

    QByteArray password() const;
    void setPassword(const QByteArray& password);

    virtual int childCount() const override;

    int indexOf(TreeItem* child) override;

    QString description() const;
    void setDescription(const QString& description);

    bool usersListed() const;
    void setUsersListed(bool usersListed);

    bool isLeaf() const override;

    void sendMessage(NetworkMessage*, TcpClient*, bool mustBeSaved);
    void sendToAll(NetworkMessage*, TcpClient* sender, bool deleteMsg= false);
    void sendToMany(NetworkMessage* msg, TcpClient* sender, bool deleteMsg= false);

    // void readFromJson(QJsonObject& json) override;
    // void writeIntoJson(QJsonObject& json) override;
    TreeItem* getChildAt(int row) override;
    const QList<QPointer<TreeItem>> childrenItem() const;

    int addChild(TreeItem*) override;

    bool addChildInto(QString id, TreeItem* child) override;

    virtual void clear() override;

    void updateNewClient(TcpClient* newComer);

    bool removeClient(TcpClient* client);
    bool removeChild(TreeItem*) override;
    bool removeChildById(const QString&);

    TcpClient* currentGM() const;
    void setCurrentGM(TcpClient* currentGM);

    QString getCurrentGmId();

    virtual void kick(const QString& str, bool isAdmin, const QString& sourceId) override;
    TreeItem* getChildById(QString id) override;
    TcpClient* getClientById(QString id);

    quint64 memorySize() const;
    void setMemorySize(quint64 size);

    bool locked() const;
    void setLocked(bool locked);

    bool contains(QString id);
public slots:
    void clearData();
    void renamePlayer(const QString& id, const QString& name);
signals:
    void memorySizeChanged(quint64 memorySize, Channel* id);
    void lockedChanged();
    void currentGMChanged();

protected:
    bool hasNoClient();
    void sendOffGmStatus(TcpClient* client);
    void findNewGM();

private:
    QByteArray m_password;
    QString m_description;
    bool m_usersListed= false;

    QList<QPointer<TreeItem>> m_child;
    QList<NetworkMessage*> m_dataToSend;
    quint64 m_memorySize= 0;
    QPointer<TcpClient> m_currentGm;
    bool m_locked= false;
};

#endif // CHANNEL_H
