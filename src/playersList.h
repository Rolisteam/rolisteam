/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
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

#include <QAbstractItemModel>

class Character;
class NetworkMessageReader;
class Liaison;
class Person;
class Player;
class ReceiveEvent;

/**
 * @brief List of connected players and theyr characters
 * @note This class is NOT thread-safe.
 */
class PlayersList : public QAbstractItemModel {
    Q_OBJECT

    public:
        /**
         * @brief Get the singleton
         */
        static PlayersList & instance();

        // implements QAbstractItemModel
        QVariant data(const QModelIndex &index, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const;
        QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role);
        void sendOffLocalPlayerInformations();
        void sendOffFeatures(Player* player);
        // Event handlers
        bool event(QEvent * event);

        // Proxy helpers
        static const quint32 NoParent = 0x7fffffff;
        QModelIndex mapIndexToMe(const QModelIndex & index) const;
        enum ItemDataRole
        {
            IdentifierRole = Qt::UserRole
        };

        // Getters
        Player * localPlayer() const;
        bool isLocal(Person * person) const;

        int numPlayers() const;
        Player * getPlayer(int index) const;

        Person * getPerson(const QString & uuid) const;
        Player * getPlayer(const QString & uuid) const;
        Character * getCharacter(const QString & uuid) const;
        Player* getLocalPlayer() const;
        void cleanList();

        /**
         * @brief Same as getPlayer(uuid), if getPerson(uuid) is a Player.
         * Same as getPerson(uuid)->parent() if it's a Character.
         */
        Player * getParent(const QString & uuid) const;

        Person * getPerson(const QModelIndex & index) const;
        Player * getPlayer(const QModelIndex & index) const;
        Character * getCharacter(const QModelIndex & index) const;


        bool everyPlayerHasFeature(const QString & name, quint8 version = 0) const;

        // Setters
        void setLocalPlayer(Player * player);
        void addLocalCharacter(Character * newCharacter);

        void changeLocalPerson(Person * person, const QString & name, const QColor & color);
        void setLocalPersonName(Person * person, const QString & name);
        void setLocalPersonColor(Person * person, const QColor & color);

        void delLocalCharacter(int index);

    signals:
        void playerAdded(Player * player);
        void characterAdded(Character * character);

        void playerChanged(Player * player);
        void characterChanged(Character * character);

        void playerDeleted(Player * player);
        void characterDeleted(Character * character);

        /**
         * @brief Send when local is client and the server refused local player to be GM.
         */
        void localGMRefused();

    private:
        QList<Player *> m_playersList;
        QMap<QString, Person *> m_uuidMap;
        Player* m_localPlayer;
        int m_gmCount;

        /**
         * @brief Constructor
         * @see instance()
         */
        PlayersList();

        /**
         * @brief Destructor
         * @see instance()
         */
        ~PlayersList();

        QModelIndex createIndex(Person * person) const;

        void addPlayer(Player * player);
        void addCharacter(Player * player, Character * character);

        void delPlayer(Player * player);
        void delCharacter(Player * parent, int index);

        void addPlayer(NetworkMessageReader & data);
        void addPlayerAsServer(ReceiveEvent * event);
        void delPlayer(NetworkMessageReader & data);
        void setPersonName(NetworkMessageReader & data);
        void setPersonColor(NetworkMessageReader & data);
        void addCharacter(NetworkMessageReader & data);
        void delCharacter(NetworkMessageReader & data);

        bool p_setLocalPersonName(Person * person, const QString & name);
        bool p_setLocalPersonColor(Person * person, const QColor & color);

        void notifyPersonChanged(Person * person);

    private slots:
        void delPlayerWithLink(Liaison * link);
        void sendDelLocalPlayer();
};

#endif
