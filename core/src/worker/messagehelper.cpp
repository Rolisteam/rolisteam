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
#include "messagehelper.h"

#include <QBuffer>

#include "data/features.h"
#include "data/player.h"
#include "dicealias.h"
#include "preferences/characterstatemodel.h"
#include "preferences/dicealiasmodel.h"

MessageHelper::MessageHelper() {}

void MessageHelper::sendOffConnectionInfo(Player* player, const QByteArray& password)
{
    if(player == nullptr)
        return;

    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::ConnectionInfo);
    msg.byteArray32(password);
    setLocalFeatures(*player);
    player->fill(msg);
    msg.sendToServer();
}

void MessageHelper::sendOffGoodBye()
{
    NetworkMessageWriter message(NetMsg::AdministrationCategory, NetMsg::Goodbye);
    message.sendToServer();
}

void MessageHelper::sendOffPlayerInformations(Player* player)
{
    NetworkMessageWriter message(NetMsg::PlayerCategory, NetMsg::PlayerConnectionAction);
    setLocalFeatures(*player);
    player->fill(message);
    message.sendToServer();
}

void MessageHelper::sendOffAllDiceAlias(DiceAliasModel* model)
{
    if(nullptr == model)
        return;

    auto aliases= model->getAliases();
    int i= 0;
    NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::DiceAliasModel);
    msg.uint32(static_cast<quint32>(i));
    for(auto& alias : *aliases)
    {
        msg.int64(i);
        msg.string32(alias->getCommand());
        msg.string32(alias->getValue());
        msg.int8(alias->isReplace());
        msg.int8(alias->isEnable());
        msg.string32(alias->getComment());
        ++i;
    }
    msg.sendToServer();
}

void MessageHelper::sendOffOneDiceAlias(DiceAlias* da, int row)
{
    if(nullptr == da)
        return;

    NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::addDiceAlias);
    msg.int64(row);
    msg.string32(da->getCommand());
    msg.string32(da->getValue());
    msg.int8(da->isReplace());
    msg.int8(da->isEnable());
    msg.string32(da->getComment());
    msg.sendToServer();
}

void MessageHelper::sendOffAllCharacterState(CharacterStateModel* model)
{
    if(nullptr == model)
        return;

    auto states= model->getCharacterStates();
    quint64 i= 0;
    NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::CharactereStateModel);
    msg.uint32(static_cast<quint32>(states->size()));
    for(auto state : *states)
    {
        msg.uint64(i);
        msg.string32(state->getLabel());
        msg.rgb(state->getColor().rgb());
        if(state->hasImage())
        {
            msg.uint8(static_cast<quint8>(true));

            QByteArray array;
            QBuffer buffer(&array);
            if(!state->getPixmap()->save(&buffer, "PNG"))
            {
                qWarning("error during encoding png");
            }
            msg.byteArray32(array);
        }
        else
        {
            msg.uint8(static_cast<quint8>(false));
        }
        ++i;
    }
    msg.sendToServer();
}

void MessageHelper::sendOffOneCharacterState(CharacterState* state, int row)
{
    if(nullptr == state)
        return;

    NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::addCharacterState);
    msg.int64(row);
    msg.string32(state->getLabel());
    msg.rgb(state->getColor().rgb());
    msg.sendToServer();
}
