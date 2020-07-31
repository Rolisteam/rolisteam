/***************************************************************************
 *	Copyright (C) 2017 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include "openmediacontroller.h"

#include "controller/contentcontroller.h"
#include "controller/media_controller/mediamanagerbase.h"
#include "data/cleveruri.h"
#include "media/mediafactory.h"
#include "model/contentmodel.h"

#include <QDebug>
#include <QUuid>

OpenMediaController::OpenMediaController(ContentModel* contentModel, Core::ContentType type,
                                         const std::map<QString, QVariant>& args, bool localIsGM, QUndoCommand* parent)
    : QUndoCommand(parent)
    , m_uuid(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_model(contentModel)
    , m_type(type)
    , m_args(args)
    , m_localIsGM(localIsGM)
{
    auto it= args.find("name");
    if(it != args.end())
        setText(QObject::tr("Open %1").arg(it->second.toString()));
}

void OpenMediaController::redo()
{
    qInfo() << QStringLiteral("Redo command OpenMediaController: %1 ").arg(text());
    if(m_model.isNull())
        return;

    auto media= Media::MediaFactory::createLocalMedia(m_uuid, m_type, m_args, m_localIsGM);
    m_model->appendMedia(media);
}

void OpenMediaController::undo()
{
    qInfo() << QStringLiteral("Undo command AddMediaContainer: %1 ").arg(text());
    if(!m_model)
        return;

    m_model->removeMedia(m_uuid);
}
