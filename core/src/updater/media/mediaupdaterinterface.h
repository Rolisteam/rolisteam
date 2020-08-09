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
#ifndef MEDIAUPDATERINTERFACE_H
#define MEDIAUPDATERINTERFACE_H

#include <QObject>

class MediaControllerBase;
class MediaUpdaterInterface : public QObject
{
    Q_OBJECT
public:
    MediaUpdaterInterface(QObject* object = nullptr);
    virtual void addMediaController(MediaControllerBase* ctrl) = 0;


    template <typename T>
    void sendOffChanges(MediaControllerBase* ctrl, const QString& property);

protected:
    bool m_updatingFromNetwork= false;
};

#endif // MEDIAUPDATERINTERFACE_H
