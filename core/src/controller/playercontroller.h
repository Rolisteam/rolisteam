/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <QObject>
#include <QPointer>
#include <QUndoStack>
#include <memory>

#include "controllerinterface.h"
#include "network/networkreceiver.h"

class Player;
class Character;
class QAbstractItemModel;
class PlayerModel;
class LocalModel;
class PlayerOnMapModel;
class CharacterStateModel;
class CharacterModel;
class PlayerController : public AbstractControllerInterface, public NetWorkReceiver
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* model READ model CONSTANT)
    Q_PROPERTY(QAbstractItemModel* playerOnMapModel READ playerOnMapModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* characterStateModel READ characterStateModel NOTIFY characterStateModelChanged)
    Q_PROPERTY(CharacterModel* characterModel READ characterModel CONSTANT)
    Q_PROPERTY(Player* localPlayer READ localPlayer WRITE setLocalPlayer NOTIFY localPlayerChanged)
    Q_PROPERTY(QString gameMasterId READ gameMasterId NOTIFY gameMasterIdChanged)

public:
    explicit PlayerController(QObject* parent= nullptr);
    ~PlayerController() override;

    Player* localPlayer() const;

    QAbstractItemModel* model() const;
    QAbstractItemModel* playerOnMapModel() const;
    QAbstractItemModel* characterStateModel() const;
    CharacterModel* characterModel() const;

    QString gameMasterId() const;

    void setGameController(GameController*) override;
    void clear();

    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;

signals:
    void localPlayerChanged();
    void characterStateModelChanged();
    void gameMasterIdChanged(const QString& gameMasterId);

public slots:
    void setLocalPlayer(Player* player);

    void addPlayer(Player* player);
    void removePlayer(Player* player);

    void addLocalCharacter();
    void removeLocalCharacter(const QModelIndex& index);

private:
    void removePlayerById(const QString& id);

private:
    std::unique_ptr<PlayerModel> m_model;
    std::unique_ptr<PlayerOnMapModel> m_playerOnMapModel;
    std::unique_ptr<CharacterModel> m_characterModel;
    QPointer<QAbstractItemModel> m_characterStateModel;
    QPointer<Player> m_localPlayer;
    QPointer<QUndoStack> m_stack;
};

#endif // PLAYERCONTROLLER_H
