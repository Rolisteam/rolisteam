/*
    Cahoots is a crossplatform real-time collaborative text editor.

    Copyright (C) 2010 Chris Dimpfl, Anandi Hira, David Vega

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PARTICIPANTSPANE_H
#define PARTICIPANTSPANE_H

#include <QAbstractProxyModel>
#include <QHostAddress>
#include <QPointer>
#include <QTreeWidgetItem>
#include <QWidget>

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "userlist/playermodel.h"

namespace Ui
{
class ParticipantsPane;
}

class ParticipantsModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Permission
    {
        readWrite,
        readOnly,
        hidden
    };
    ParticipantsModel(QObject* parent= nullptr);
    virtual int rowCount(const QModelIndex& parent) const;
    virtual int columnCount(const QModelIndex& index) const;
    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual QModelIndex parent(const QModelIndex& child) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent) const;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;

    /*Q_INVOKABLE QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
    Q_INVOKABLE QModelIndex mapToSource(const QModelIndex& proxyIndex) const;*/

    QString getOwner() const;
    void setOwner(const QString& owner);

    void saveModel(QJsonObject& root);
    QList<Player*>* getListByChild(Player* owner);

    ParticipantsModel::Permission getPermissionFor(const QModelIndex& index);
    void loadModel(QJsonObject& root);
public slots:
    virtual void addHiddenPlayer(Player*);
    virtual void removePlayer(Player*);
    int promotePlayer(const QModelIndex& index);
    int demotePlayer(const QModelIndex& index);

    void setPlayerInto(const QModelIndex& index, ParticipantsModel::Permission level);

private:
    // void debugModel() const;
    QPointer<PlayerModel> m_playerList;
    std::vector<QString> m_readOnly;
    std::vector<QString> m_readWrite;
    QStringList m_permissionGroup;
    QString m_ownerId;
};

class ParticipantsPane : public QWidget
{
    Q_OBJECT
public:
    ParticipantsPane(QWidget* parent= nullptr);
    virtual ~ParticipantsPane();

    bool canWrite(Player* idPlayer);
    bool canRead(Player* idPlayer);
    void setFont(QFont font);
    bool isOwner() const;

    void fill(NetworkMessageWriter* msg);
    void readFromMsg(NetworkMessageReader* msg);

    Player* getOwner() const;
    void setOwnerId(const QString& id);

    void readPermissionChanged(NetworkMessageReader* msg);

signals:
    void memberCanNowRead(QString name);
    void memberPermissionsChanged(QString id, int i);
    void closeMediaToPlayer(QString id);
    void localPlayerPermissionChanged(ParticipantsModel::Permission);
    void localPlayerIsOwner(bool);

private slots:
    void promoteCurrentItem();
    void demoteCurrentItem();
    void addNewPlayer(Player*);

private:
    Ui::ParticipantsPane* ui;

    PlayerModel* m_playerList;
    ParticipantsModel* m_model;
};

#endif // PARTICIPANTSPANE_H
