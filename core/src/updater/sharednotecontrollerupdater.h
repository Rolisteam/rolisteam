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
#ifndef SHAREDNOTECONTROLLERUPDATER_H
#define SHAREDNOTECONTROLLERUPDATER_H

#include <QObject>
#include <map>

#include "mediaupdaterinterface.h"

class SharedNoteController;
class NetworkMessageReader;
class SharedNoteControllerUpdater : public MediaUpdaterInterface
{
    Q_OBJECT

public:
    explicit SharedNoteControllerUpdater(QObject* parent= nullptr);

    void addMediaController(MediaControllerBase* ctrl) override;
    void addSharedNoteController(SharedNoteController* sheet);

    void updateProperty(NetworkMessageReader* msg, SharedNoteController* ctrl);
    void sendOffPermissionChanged(SharedNoteController* ctrl, bool b, const QString& id);
signals:

private:
    std::map<SharedNoteController*, QSet<QString>> m_noteReaders;
    bool m_updatingFromNetwork= false;
};

#endif // SHAREDNOTECONTROLLERUPDATER_H
