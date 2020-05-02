/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PARTICIPANTMODEL_H
#define PARTICIPANTMODEL_H

#include <QAbstractItemModel>
#include <QPointer>

class Player;

class ParticipantItem
{
public:
    ParticipantItem(const QString& name);
    ParticipantItem(Player* player);

    bool isLeaf() const;
    QString name() const;
    QString uuid() const;
    Player* player() const;

    int indexOf(ParticipantItem* child);
    int childCount() const;
    ParticipantItem* childAt(int pos);

    void appendChild(ParticipantItem* item);
    void removeChild(ParticipantItem* item);

    ParticipantItem* parent() const;
    void setParent(ParticipantItem* parent);

private:
    QString m_name;
    QPointer<Player> m_player;
    ParticipantItem* m_parent= nullptr;
    QList<ParticipantItem*> m_children;
};

class PlayerModel;
class ParticipantModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Permission
    {
        readWrite= 0,
        readOnly,
        hidden
    };
    Q_ENUM(Permission)
    explicit ParticipantModel(const QString& ownerId, PlayerModel* model, QObject* parent= nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const override;

    // Basic functionality
    QModelIndex index(int row, int column, const QModelIndex& parent= QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    int columnCount(const QModelIndex& parent= QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    QString getOwner() const;
    void setOwner(const QString& owner);

    ParticipantModel::Permission permissionFor(const QModelIndex& index);
    ParticipantModel::Permission permissionFor(const QString& id);

    void saveModel(QJsonObject& root);
    void loadModel(QJsonObject& root);

public slots:
    virtual void addNewPlayer(Player*);
    virtual void removePlayer(Player*);
    int promotePlayer(const QModelIndex& index);
    int demotePlayer(const QModelIndex& index);
    void setPlayerInto(const QModelIndex& index, ParticipantModel::Permission level);
    void setPlayerPermission(const QString& id, ParticipantModel::Permission level);
    void initModel();

signals:
    void userReadPermissionChanged(QString, bool);
    void userWritePermissionChanged(QString, bool);

private:
    QPointer<PlayerModel> m_playerList;
    std::unique_ptr<ParticipantItem> m_root;
    QString m_ownerId;
};

#endif // PARTICIPANTMODEL_H
