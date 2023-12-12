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
#include "worker/characterfinder.h"

#include "model/charactermodel.h"
#include "model/playermodel.h"
#include "model/nonplayablecharactermodel.h"

QPointer<campaign::NonPlayableCharacterModel> CharacterFinder::m_npcModel
    = QPointer<campaign::NonPlayableCharacterModel>(nullptr);
QPointer<CharacterModel> CharacterFinder::m_pcModel= QPointer<CharacterModel>(nullptr);
QPointer<PlayerModel> CharacterFinder::m_playerModel= QPointer<PlayerModel>(nullptr);


bool CharacterFinder::setUpConnect()
{
    if(!isReady())
        return false;

    connect(m_playerModel, &PlayerModel::playerJoin, this, &CharacterFinder::dataChanged);
    return true;
}
bool CharacterFinder::isReady()
{
    return (m_pcModel && m_npcModel && m_playerModel);
}
void CharacterFinder::setNpcModel(campaign::NonPlayableCharacterModel* model)
{
    m_npcModel= model;
}
void CharacterFinder::setPcModel(CharacterModel* model)
{
    m_pcModel= model;
}

void CharacterFinder::setPlayerModel(PlayerModel* model)
{
    m_playerModel = model;
}

Character* CharacterFinder::find(const QString& id)
{
    if(!isReady())
        return nullptr;

    auto res= m_pcModel->character(id);

    if(res == nullptr)
        res= m_npcModel->characterFromUuid(id);

    return res;
}
