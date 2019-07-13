/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *      Copyright (C) 2014 by Renaud Guezennec                            *
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
class NetworkLink;
class Person;
class Player;
class ReceiveEvent;

/**
 * @brief PlayersList is a model of players and character. List of connected players and theyr characters
 * @note This class is NOT thread-safe.
 */
class PlayersList : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum ItemDataRole
    {
        IdentifierRole= Qt::UserRole + 1,
        PersonPtr,
        IsLocal,
        IsGM,
        LocalIsGM
    };
    /**
     * @brief Get the singleton
     */
    static PlayersList* instance();

    ////////////////////////////////////
    // implements QAbstractItemModel
    ///////////////////////////////////
    QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex& parent= QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index) const;
    int rowCount(const QModelIndex& parent= QModelIndex()) const;
    int columnCount(const QModelIndex& parent= QModelIndex()) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);
    void sendOffLocalPlayerInformations();
    void sendOffFeatures(Player* player);

    // Event handlers
    bool event(QEvent* event);

    // Getters
    /**
     * @brief isLocal
     * @param person
     * @return  true if the person is the local user or one of his children.
     */
    bool isLocal(Person* person) const;
    int getPlayerCount() const;
    Player* getPlayer(int index) const;
    Person* getPerson(const QString& uuid) const;
    Player* getPlayer(const QString& uuid) const;
    Character* getCharacter(const QString& uuid) const;
    Player* getLocalPlayer() const;
    /**
     * @brief Same as getPlayer(uuid), if getPerson(uuid) is a Player.
     * Same as getPerson(uuid)->parent() if it's a Character.
     */
    Player* getParent(const QString& uuid) const;
    Person* getPerson(const QModelIndex& index) const;
    Player* getPlayer(const QModelIndex& index) const;
    Character* getCharacter(const QModelIndex& index) const;

    /**
     * @brief getUuidFromName
     * @param name
     * @return uuid of the given name. If it has not been found, return uuid of local person.
     */
    QString getUuidFromName(QString name);

    QList<Character*> getCharacterList();

    void cleanListButLocal();
    void completeListClean();
    bool everyPlayerHasFeature(const QString& name, quint8 version= 0) const;

    // Setters
    void setLocalPlayer(Player* player);
    void addLocalCharacter(Character* newCharacter);
    void changeLocalPerson(Person* person, const QString& name, const QColor& color, const QImage& image);
    void delLocalCharacter(int index);

    bool hasPlayer(Player* player);
    // static void defineFeaturePlayer(Player* player);

    void addNpc(Character* character);
    bool localIsGM() const;

    Player* getGM();
    QString getGmId();
    QString getLocalPlayerId() const;
public slots:
    void setCurrentGM(QString idGm);
signals:
    void playerAdded(Player* player);
    void characterAdded(Character* character);
    void playerDeleted(Player* player);
    void characterDeleted(Character* character);
    void playerAddedAsClient(Player* player);
    void eventOccurs(const QString msg);
    /**
     * @brief Send when local is client and the server refused local player to be GM.
     */
    void localGMRefused(bool);

protected:
    template <typename T>
    void sendOffPersonChanges(const QString& property);
protected slots:
    void updatePerson(NetworkMessageReader& data);

private:
    /**
     * @brief Constructor
     * @see instance()
     */
    PlayersList();
    /**
     * @brief Destructor
     * @see instance()
     */
    virtual ~PlayersList();
    QModelIndex personToIndex(Person* person) const;
    void addPlayer(Player* player);
    void addCharacter(Player* player, Character* character);
    void delPlayer(Player* player);
    void delCharacter(Player* parent, int index);
    void receivePlayer(NetworkMessageReader& data);
    void delPlayer(NetworkMessageReader& data);
    void addCharacter(NetworkMessageReader& data);
    void delCharacter(NetworkMessageReader& data);
    void monitorCharacter(Character* charac);
    void monitorPlayer(Player* player);

private:
    QList<Player*> m_playersList;
    QMap<QString, Person*> m_uuidMap;
    /// WARNING ugly solution to store NPC
    QList<Character*> m_npcList;

    static PlayersList* m_singleton;
    Player* m_localPlayer= nullptr;
    QString m_idCurrentGM;
    bool m_receivingData= false;
};

#endif
