/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#ifndef CHARACTERFINDER_H
#define CHARACTERFINDER_H

#include <core_global.h>

#include <QPointer>
#include <QObject>

namespace campaign {
class NonPlayableCharacterModel;
}
class CharacterModel;
class Character;
class PlayerModel;

/**
 * @brief The CharacterFinder class - helper to find a character given its uuid in PC or NPC models.
 */
class CORE_EXPORT CharacterFinder : public QObject
{
    Q_OBJECT
public:
    CharacterFinder() = default;

    bool isReady();

    Character *find(const QString &id);

    static void setNpcModel(campaign::NonPlayableCharacterModel *model);
    static void setPcModel(CharacterModel *model);
    static void setPlayerModel(PlayerModel *model);

    bool setUpConnect();
signals:
    void dataChanged();
    void characterAdded(const QStringList &ids);

private slots:

private:
    static QPointer<campaign::NonPlayableCharacterModel> m_npcModel;
    static QPointer<CharacterModel> m_pcModel;
    static QPointer<PlayerModel> m_playerModel;
};

#endif // CHARACTERFINDER_H
