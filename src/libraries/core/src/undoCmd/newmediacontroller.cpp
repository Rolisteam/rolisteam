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
#include "undoCmd/newmediacontroller.h"

#include <QDebug>
#include <QFileInfo>
#include <QUuid>

#include "controller/contentcontroller.h"
#include "data/campaign.h"
#include "data/campaigneditor.h"
#include "media/mediafactory.h"
#include "model/contentmodel.h"
#include "worker/iohelper.h"
#include "worker/utilshelper.h"

NewMediaController::NewMediaController(ContentModel* model, const std::map<QString, QVariant>& map, QColor color,
                                       bool localIsGM, campaign::CampaignEditor* editor, QUndoCommand* parent)
    : QUndoCommand(parent), m_color(color), m_args(map), m_model(model), m_localGM(localIsGM), m_editor(editor)
{
    m_contentType= m_args[Core::keys::KEY_TYPE].value<Core::ContentType>();
    m_title= m_args[Core::keys::KEY_NAME].toString();
    m_uuidUri= m_args[Core::keys::KEY_UUID].toString();
    m_fullPath= m_args[Core::keys::KEY_URL].toUrl();

    if(m_uuidUri.isEmpty())
        m_uuidUri= QUuid::createUuid().toString(QUuid::WithoutBraces);

    if(m_title.isEmpty())
    {
        m_title= QString("%1_%2")
                     .arg(helper::utils::typeToString(m_contentType))
                     .arg(model->mediaCount(m_contentType) + 1);
    }

    if(m_fullPath.isEmpty())
    {
        m_fullPath
            = QUrl::fromUserInput(m_editor ? m_editor->mediaFullPathWithExtension(m_title, m_contentType) : m_title);

        if(QFileInfo::exists(m_fullPath.toLocalFile()))
        {
            m_fullPath= QUrl::fromUserInput(m_editor ? m_editor->mediaFullPathWithExtension(m_uuidUri, m_contentType) :
                                                       m_title);
        }
    }

    QString media(QObject::tr("Create new %1 %2"));

    setText(media.arg(helper::utils::typeToString(m_contentType), m_title));
}

void NewMediaController::redo()
{
    if(m_model.isNull())
        return;

    qInfo() << QStringLiteral("Redo command newmediacontroller: %1 ").arg(text());
    auto media= Media::MediaFactory::createLocalMedia(m_uuidUri, m_contentType, m_args, m_color, m_localGM);
    media->setUrl(m_fullPath);
    if(m_editor)
    {
        m_editor->addMedia(m_uuidUri, m_fullPath.toLocalFile(), IOHelper::saveController(media));
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
        m_editor->removeMedia(m_fullPath.toLocalFile());
    }
}
