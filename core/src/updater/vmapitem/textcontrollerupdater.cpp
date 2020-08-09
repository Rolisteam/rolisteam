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
#include "textcontrollerupdater.h"

#include "network/networkmessagereader.h"
#include "vmap/controller/textcontroller.h"

TextControllerUpdater::TextControllerUpdater(QObject* parent) : VMapItemControllerUpdater(parent) {}

void TextControllerUpdater::addTextController(vmap::TextController* ctrl)
{
    if(nullptr == ctrl)
        return;

    VMapItemControllerUpdater::addItemController(ctrl);

    connect(ctrl, &vmap::TextController::textChanged, this,
            [this, ctrl]() { sendOffVMapChanges<QString>(ctrl, QStringLiteral("text")); });
    connect(ctrl, &vmap::TextController::borderRectEditFinished, this,
            [this, ctrl]() { sendOffVMapChanges<QRectF>(ctrl, QStringLiteral("borderRect")); });
    connect(ctrl, &vmap::TextController::fontChanged, this,
            [this, ctrl]() { sendOffVMapChanges<QFont>(ctrl, QStringLiteral("font")); });
    connect(ctrl, &vmap::TextController::textPosChanged, this,
            [this, ctrl]() { sendOffVMapChanges<QPointF>(ctrl, QStringLiteral("textPos")); });
}

bool TextControllerUpdater::updateItemProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl)
{
    if(nullptr == msg || nullptr == ctrl)
        return false;

    // TODO implement save/restore
    auto datapos= msg->pos();

    if(VMapItemControllerUpdater::updateItemProperty(msg, ctrl))
        return true;

    msg->resetToPos(datapos);

    updatingCtrl= ctrl;

    auto property= msg->string16();

    QVariant var;

    if(property == QStringLiteral("text"))
    {
        var= QVariant::fromValue(msg->string32());
    }
    else if(property == QStringLiteral("borderRect"))
    {
        auto x= msg->real();
        auto y= msg->real();
        auto w= msg->real();
        auto h= msg->real();
        var= QVariant::fromValue(QRectF(x, y, w, h));
    }
    else if(property == QStringLiteral("font"))
    {
        QFont font;
        font.fromString(msg->string16());
        var= QVariant::fromValue(font);
    }
    else if(property == QStringLiteral("textPos"))
    {
        auto x= msg->real();
        auto y= msg->real();
        var= QVariant::fromValue(QPointF(x, y));
    }
    m_updatingFromNetwork= true;
    auto feedback= ctrl->setProperty(property.toLocal8Bit().data(), var);
    m_updatingFromNetwork= false;
    updatingCtrl= nullptr;

    return feedback;
}
