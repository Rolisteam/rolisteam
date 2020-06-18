/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *     Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
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

#ifndef PLAYERS_LIST_H
#define PLAYERS_LIST_H

//#include "data/player.h"
#include <QAbstractItemModel>
#include <QPointer>
#include <memory>

class Character;
class Player;
class NetworkMessageReader;
class NetworkLink;
class Person;
class ReceiveEvent;
/**
 * @brief PlayersList is a model of players and character. List of connected players and theyr characters
 * @note This class is NOT thread-safe.
 */
class PlayerModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(QString gameMasterId READ gameMasterId NOTIFY gameMasterIdChanged)
    Q_PROPERTY(QString localPlayerId READ localPlayerId WRITE setLocalPlayerId NOTIFY localPlayerIdChanged)
public:
    enum ItemDataRole
    {
        IdentifierRole= Qt::UserRole + 1,
        PersonPtrRole,
        NameRole,
        LocalRole,
        GmRole,
        CharacterRole,
        CharacterStateIdRole,
        NpcRole,
        AvatarRole
    };

    PlayerModel(QObject* parent= nullptr);
    virtual ~PlayerModel() override;

    ////////////////////////////////////
    // implements QAbstractItemModel
    ///////////////////////////////////
    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent= QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    int columnCount(const QModelIndex& parent= QModelIndex()) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    Player* playerById(const QString& id) const;
    Person* personById(const QString& id) const;

    QModelIndex personToIndex(Person* person) const;

    QString gameMasterId() const;
    QString localPlayerId() const;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void clear();
    void addPlayer(Player* player);
    void removePlayer(Player* player);
    void addCharacter(const QModelIndex& parent, Character* character, int pos= -1);
    void removeCharacter(Character* character);
    void setLocalPlayerId(const QString& uuid);

signals:
    void playerJoin(Player* player);
    void playerLeft(Player* player);
    void gameMasterIdChanged(const QString& gameMasterId);
    void localPlayerIdChanged(const QString& localId);

private:
    void setGameMasterId(const QString& id);

private:
    std::vector<std::unique_ptr<Player>> m_players;
    QString m_gameMasterId;
    QString m_localPlayerId;
};

#endif
