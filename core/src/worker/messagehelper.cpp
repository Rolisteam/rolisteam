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
#include <QDebug>

#include "controller/view_controller/abstractmediacontroller.h"
#include "controller/view_controller/imagecontroller.h"
#include "data/character.h"
#include "data/cleveruri.h"
#include "data/player.h"
#include "dicealias.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "preferences/characterstatemodel.h"
#include "preferences/dicealiasmodel.h"
#include "userlist/playermodel.h"

MessageHelper::MessageHelper() {}

void MessageHelper::sendOffGoodBye()
{
    NetworkMessageWriter message(NetMsg::AdministrationCategory, NetMsg::Goodbye);
    message.sendToServer();
}

QString MessageHelper::readPlayerId(NetworkMessageReader& msg)
{
    return msg.string8();
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

void MessageHelper::sendOffOpenMedia(AbstractMediaController* ctrl) {}

void MessageHelper::sendOffImage(ImageController* ctrl)
{
    if(nullptr == ctrl)
        return;
    auto uri= ctrl->uri();

    if(nullptr == uri)
        return;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::addMedia);
    msg.uint8(static_cast<quint8>(uri->getType()));
    msg.string16(ctrl->name());
    msg.string8(ctrl->uuid());
    msg.string8(uri->ownerId());
    auto img= ctrl->pixmap();
    QByteArray array;
    QBuffer imageData(&array);
    if(!img.save(&imageData, "jpg", 70))
    {
        // TODO log error
    }
    msg.byteArray32(array);
}

void MessageHelper::updatePerson(NetworkMessageReader& data, PlayerModel* playerModel)
{
    QString uuid= data.string8();
    auto person= playerModel->playerById(uuid);
    if(nullptr == person)
        return;

    auto role= data.int32();

    auto property= data.string8();
    QVariant var;
    if(property.contains("color"))
    {
        var= QVariant::fromValue(QColor(data.rgb()));
    }
    else if(property.contains("avatar"))
    {
        auto array= data.byteArray32();
        QDataStream out(&array, QIODevice::ReadOnly);
        out.setVersion(QDataStream::Qt_5_7);
        QImage img;
        out >> img;
        var= QVariant::fromValue(img);
    }
    else if(property.contains(QStringLiteral("state")))
    {
        auto label= data.string32();
        auto state= Character::getStateFromLabel(label);
        var= QVariant::fromValue(state);
    }
    else
    {
        auto val= data.string32();
        var= QVariant::fromValue(val);
    }

    auto idx= playerModel->personToIndex(person);

    // person->setProperty(property.toLocal8Bit().data(), var);
    playerModel->setData(idx, var, role);
    /*
        auto idx= personToIndex(person);
        emit dataChanged(idx, idx);*/
}
