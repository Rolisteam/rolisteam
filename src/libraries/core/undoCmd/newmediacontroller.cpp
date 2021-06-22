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
#include "data/campaigneditor.h"
#include "data/cleveruri.h"
#include "media/mediafactory.h"
#include "model/contentmodel.h"

#include "worker/fileserializer.h"
#include "worker/iohelper.h"

NewMediaController::NewMediaController(ContentModel* model, const std::map<QString, QVariant>& map, bool localIsGM,
                                       campaign::CampaignEditor* editor, QUndoCommand* parent)
    : QUndoCommand(parent), m_args(map), m_model(model), m_localGM(localIsGM), m_editor(editor)
{
    m_contentType= m_args[Core::keys::KEY_TYPE].value<Core::ContentType>();
    m_title= m_args[Core::keys::KEY_NAME].toString();

    m_fullPath= m_editor ? m_editor->mediaFullPath(m_title, m_contentType) : m_title;

    QString media(QObject::tr("Create new %1 %2"));

    setText(media.arg(CleverURI::typeToString(m_contentType), m_title));
}

void NewMediaController::redo()
{
    if(m_model.isNull())
        return;

    if(m_uuidUri.isEmpty())
        m_uuidUri= QUuid::createUuid().toString(QUuid::WithoutBraces);

    qInfo() << QStringLiteral("Redo command newmediacontroller: %1 ").arg(text());
    auto media= Media::MediaFactory::createLocalMedia(m_uuidUri, m_contentType, m_args, m_localGM);
    media->setPath(m_fullPath);
    if(m_editor)
    {
        m_editor->addMedia(m_fullPath, IOHelper::saveController(media));
    }
    m_model->appendMedia(media);
}

void NewMediaController::undo()
{
    if(m_model.isNull())
        return;
    qInfo() << QStringLiteral("Undo command newmediacontroller: %1 ").arg(text());
    m_model->removeMedia(m_uuidUri);
    if(m_editor)
    {
        m_editor->removeMedia(m_fullPath);
    }
}
