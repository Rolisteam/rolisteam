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
#ifndef ADDMEDIACONTENEUR_H
#define ADDMEDIACONTENEUR_H

#include <QPointer>
#include <QUndoCommand>

#include "controller/contentcontroller.h"
#include "data/mediacontainer.h"

class Workspace;

class AddMediaContainer : public QUndoCommand
{
public:
    AddMediaContainer(MediaContainer* mediac, ContentController* ctrl, QMenu* menu, Workspace* workspace, bool isGM,
                      QUndoCommand* parent= nullptr);

    void redo() override;
    void undo() override;

    bool sendAtOpening();

private:
    MediaContainer* m_media= nullptr;
    QPointer<ContentController> m_ctrl= nullptr;
    QMenu* m_menu= nullptr;
    Workspace* m_mdiArea= nullptr;
    bool m_gm;
};

#endif // ADDMEDIACONTENEUR_H
