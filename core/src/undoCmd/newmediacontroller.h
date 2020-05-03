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
#ifndef NEWMEDIACONTROLLER_H
#define NEWMEDIACONTROLLER_H

#include "media/mediatype.h"
#include <QPointer>
#include <QUndoCommand>
#include <memory>

class MediaManagerBase;
class ContentController;
class NewMediaController : public QUndoCommand
{
public:
    NewMediaController(Core::ContentType contentType, MediaManagerBase* ctrl, ContentController* contentCtrl,
                       const std::map<QString, QVariant>& map, QUndoCommand* parent= nullptr);

    void redo() override;
    void undo() override;

private:
    QPointer<MediaManagerBase> m_ctrl;
    QString m_uuidUri;
    QString m_title;
    Core::ContentType m_contentType;
    QPointer<ContentController> m_contentCtrl;
    std::map<QString, QVariant> m_args;
    // CleverURI* m_uri= nullptr;
};

#endif // NEWMEDIACONTROLLER_H
