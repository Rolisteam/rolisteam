/***************************************************************************
 *	Copyright (C) 2017 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                                      *
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
#ifndef REMOVEMEDIACONTROLLERCOMMAND_H
#define REMOVEMEDIACONTROLLERCOMMAND_H

#include <QUndoCommand>
#include <map>
#include <QVariant>

#include "data/cleveruri.h"

class MediaControllerBase;
class MediaManagerBase;
class RemoveMediaControllerCommand : public QUndoCommand
{
public:
    RemoveMediaControllerCommand(MediaControllerBase* media, MediaManagerBase* ctrl, QUndoCommand* parent= nullptr);

    ~RemoveMediaControllerCommand() override;

    void redo() override;
    void undo() override;

private:
    QString m_uuid;
    QString m_title;
    MediaControllerBase* m_ctrl = nullptr;
    MediaManagerBase* m_manager;
    std::map<QString, QVariant> m_params;
    Core::ContentType m_contentType;
};

#endif // REMOVEMEDIACONTROLLERCOMMAND_H
