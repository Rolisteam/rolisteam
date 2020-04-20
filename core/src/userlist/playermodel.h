/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *     Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     https://rolisteam.org/                                         *
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
    /**
     * @brief Constructor
     * @see instance()
     */
    PlayerModel(QObject* parent= nullptr);
    /**
     * @brief Destructor
     * @see instance()
     */
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

    // void sendOffLocalPlayerInformations();
    // void sendOffFeatures(Player* player);

    // Event handlers
    // bool event(QEvent* event) override;

    //    const std::vector<std::unique_ptr<Character>>& getCharacterList() const;

public slots:
    void clear();
    void addPlayer(Player* player);
    void removePlayer(Player* player);
    void addCharacter(const QModelIndex& parent, Character* character, int pos= -1);
    void removeCharacter(Character* character);

signals:
    void playerJoin(Player* player);
    void playerLeft(Player* player);
    void gameMasterIdChanged(const QString& gameMasterId);
    void localPlayerChanged(const QString& localId);

private:
    void setGameMasterId(const QString& id);

    //    bool everyPlayerHasFeature(const QString& name, quint8 version= 0) const;

    // Setters
    // void changeLocalPerson(Person* person, const QString& name, const QColor& color, const QImage& image);
    // void delLocalCharacter(int index);

    // bool hasPlayer(Player* player);
    // static void defineFeaturePlayer(Player* player);

    // void addNpc(Character* character);
    // bool localIsGM() const;

    /*signals:
        void playerAdded(Player* player);
        void characterAdded(Character* character);
        void playerDeleted(Player* player);
        void characterDeleted(Character* character);
        void playerAddedAsClient(Player* player);
        void eventOccurs(const QString msg);
        void localGMRefused(bool);
        void localPlayerChanged();*/

    /*protected:
        template <typename T>
        void sendOffPersonChanges(const QString& property);
    protected slots:
        void updatePerson(NetworkMessageReader& data);*/

private:
    /* void addPlayer(Player* player);
     void addCharacter(Player* player, Character* character);
     void delPlayer(Player* player);
     void delCharacter(Player* parent, int index);
     void receivePlayer(NetworkMessageReader& data);
     void delPlayer(NetworkMessageReader& data);
     void addCharacter(NetworkMessageReader& data);
     void delCharacter(NetworkMessageReader& data);
     void monitorCharacter(Character* charac);
     void monitorPlayer(Player* player);*/

private:
    std::vector<std::unique_ptr<Player>> m_players;
    QString m_gameMasterId;
    //    bool m_receivingData= false;
};

#endif
