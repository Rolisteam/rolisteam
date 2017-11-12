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

#include <QWidget>
#include <QTreeWidgetItem>
#include <QHostAddress>
#include <QTcpSocket>

#include "userlist/playersList.h"
#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

namespace Ui {
    class ParticipantsPane;
}

class ParticipantsModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Permission {readWrite,readOnly,hidden};
    ParticipantsModel(PlayersList* m_playerList);
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual QModelIndex index(int row, int column, const QModelIndex & parent) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    Player* getOwner() const;
    void setOwner(Player *owner);

    void saveModel(QJsonObject& root);
    QList<Player *> *getListByChild(Player *owner);

    ParticipantsModel::Permission getPermissionFor(Player *player);
    void loadModel(QJsonObject& root);
public slots:
    virtual void addHiddenPlayer(Player*);
    virtual void removePlayer(Player*);
    int promotePlayer(Player*);
    int demotePlayer(Player*);

    void setPlayerInto(Player *player, Permission level);
private:
    void debugModel() const;
    QList<Player*> m_hidden;
    QList<Player*> m_readOnly;
    QList<Player*> m_readWrite;

    QList<QList<Player*>*> m_data;
    QStringList m_permissionGroup;

    Player* m_owner;
};

class ParticipantsPane : public QWidget
{
    Q_OBJECT
public:
    ParticipantsPane(QWidget *parent = 0);
    virtual ~ParticipantsPane();

    bool canWrite(Player* idPlayer);
    bool canRead(Player* idPlayer);
    void setFont(QFont font);
    bool isOwner() const;

    void fill(NetworkMessageWriter* msg);
    void readFromMsg(NetworkMessageReader *msg);

    Player* getOwner() const;
    void setOwner(Player* owner);

    void readPermissionChanged(NetworkMessageReader *msg);

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
    void removePlayer(Player *player);

private:
    Ui::ParticipantsPane* ui;

    PlayersList* m_playerList;
    ParticipantsModel* m_model;
};

#endif // PARTICIPANTSPANE_H
