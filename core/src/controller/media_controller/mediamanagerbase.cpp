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
#include "mediamanagerbase.h"

#include "model/contentmodel.h"
#include "model/singlecontenttypemodel.h"
#include <QUndoStack>

MediaManagerBase::MediaManagerBase(Core::ContentType contentType, ContentModel* contentModel, QObject* parent)
    : QObject(parent), m_model(new SingleContentTypeModel(contentType))
{
    m_model->setSourceModel(contentModel);
}

MediaManagerBase::~MediaManagerBase()= default;

Core::ContentType MediaManagerBase::type() const
{
    return m_model->type();
}

int MediaManagerBase::managerCount() const
{
    return m_model->rowCount();
}

NetWorkReceiver::SendType MediaManagerBase::processMessage(NetworkMessageReader*)
{
    return NetWorkReceiver::NONE;
}

void MediaManagerBase::setLocalIsGM(bool localIsGM)
{
    if(m_localIsGM == localIsGM)
        return;

    m_localIsGM= localIsGM;
    emit localIsGMChanged(m_localIsGM);
}

void MediaManagerBase::setUndoStack(QUndoStack* stack)
{
    m_undoStack= stack;
}

bool MediaManagerBase::localIsGM() const
{
    return m_localIsGM;
}

QString MediaManagerBase::localId() const
{
    return m_localId;
}

SingleContentTypeModel* MediaManagerBase::model() const
{
    return m_model.get();
}

void MediaManagerBase::setLocalId(const QString& id)
{
    if(m_localId == id)
        return;
    m_localId= id;

    emit localIdChanged(m_localId);
}
