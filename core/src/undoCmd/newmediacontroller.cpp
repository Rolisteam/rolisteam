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
#include "newmediacontroller.h"

#include <QDebug>
#include <QUuid>

#include "controller/contentcontroller.h"
#include "controller/media_controller/mediamanagerbase.h"
#include "media/mediafactory.h"
#include "model/contentmodel.h"

NewMediaController::NewMediaController(Core::ContentType contentType, ContentModel* model,
                                       const std::map<QString, QVariant>& map, bool localIsGM, QUndoCommand* parent)
    : QUndoCommand(parent), m_contentType(contentType), m_args(map), m_model(model), m_localGM(localIsGM)
{
    m_title= m_args["name"].toString();

    QString media(QObject::tr("Create new %1 %2"));

    setText(media.arg(CleverURI::typeToString(contentType)).arg(m_title));
}

void NewMediaController::redo()
{
    if(m_model.isNull())
        return;

    if(m_uuidUri.isEmpty())
        m_uuidUri= QUuid::createUuid().toString(QUuid::WithoutBraces);

    qInfo() << QStringLiteral("Redo command newmediacontroller: %1 ").arg(text());
    auto media= Media::MediaFactory::createLocalMedia(m_uuidUri, m_contentType, m_args, m_localGM);
    m_model->appendMedia(media);
}

void NewMediaController::undo()
{
    if(m_model.isNull())
        return;
    qInfo() << QStringLiteral("Undo command newmediacontroller: %1 ").arg(text());
    m_model->removeMedia(m_uuidUri);
}
